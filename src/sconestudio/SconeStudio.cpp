/*
** SconeStudio.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SconeStudio.h"
#include "StudioSettings.h"
#include "studio_config.h"
#include "ui_SconeSettings.h"

#include "scone/core/system_tools.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"
#include "scone/core/Settings.h"
#include "scone/core/Factories.h"
#include "scone/optimization/Optimizer.h"
#include "scone/optimization/opt_tools.h"

#include "vis/plane.h"
#include "vis-osg/osg_tools.h"
#include "vis-osg/osg_object_manager.h"

#include "xo/utility/color.h"
#include "xo/filesystem/filesystem.h"
#include "xo/system/system_tools.h"
#include "xo/container/container_tools.h"

#include <osgDB/ReadFile>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QTabWidget>
#include "qcustomplot.h"
#include "qt_convert.h"
#include "xo/container/prop_node_tools.h"
#include "scone/model/muscle_tools.h"
#include "scone/core/storage_tools.h"
#include "scone/core/StorageIo.h"
#include "GaitAnalysis.h"
#include "OptimizerTaskExternal.h"
#include "OptimizerTaskThreaded.h"

using namespace scone;
using namespace xo::literals;

SconeStudio::SconeStudio( QWidget* parent, Qt::WindowFlags flags ) :
	QCompositeMainWindow( parent, flags ),
	close_all( false ),
	current_time(),
	evaluation_time_step( 1.0 / 8 ),
	scene_( true, GetStudioSetting< float >( "viewer.ambient_intensity" ) ),
	slomo_factor( 1 ),
	com_delta( Vec3( 0, 0, 0 ) ),
	captureProcess( nullptr )
{
	xo::log::debug( "Constructing UI elements" );
	ui.setupUi( this );

	// analysis
	analysisView = new QDataAnalysisView( &analysisStorageModel, this );
	analysisView->setObjectName( "Analysis" );
	analysisView->setMinSeriesInterval( 0 );
	analysisView->setLineWidth( scone::GetStudioSettings().get< float >( "analysis.line_width" ) );

	// menu
	createFileMenu( to_qt( GetFolder( SCONE_SCENARIO_FOLDER ) ), "Scone Scenario (*.scone)" );

	auto editMenu = menuBar()->addMenu( "&Edit" );
	editMenu->addAction( "&Find...", this, &SconeStudio::findDialog, QKeySequence( "Ctrl+F" ) );
	editMenu->addAction( "Find &Next", this, &SconeStudio::findNext, Qt::Key_F3 );
	editMenu->addAction( "Find &Previous", this, &SconeStudio::findPrevious, QKeySequence( "Shift+F3" ) );
	editMenu->addSeparator();
	editMenu->addAction( "&Toggle Comments", this, &SconeStudio::toggleComments, QKeySequence( "Ctrl+/" ) );

	auto viewMenu = menuBar()->addMenu( "&View" );
	viewActions[ ModelVis::ShowForces ] = viewMenu->addAction( "Show External &Forces", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowMuscles ] = viewMenu->addAction( "Show &Muscles", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowTendons ] = viewMenu->addAction( "Show &Tendons", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowBodyGeom ] = viewMenu->addAction( "Show &Body Geometry", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowBodyAxes ] = viewMenu->addAction( "Show Body &Axes", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowBodyCom ] = viewMenu->addAction( "Show Body Cente&r of Mass", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowJoints ] = viewMenu->addAction( "Show &Joints", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowContactGeom ] = viewMenu->addAction( "Show &Contact Geometry", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowGroundPlane ] = viewMenu->addAction( "Show &Ground Plane", this, &SconeStudio::updateViewSettings );
	for ( auto& va : viewActions )
	{
		va.second->setCheckable( true );
		va.second->setChecked( va.first != ModelVis::ShowBodyAxes && va.first != ModelVis::ShowJoints && va.first != ModelVis::ShowBodyCom );
	}

	auto scenarioMenu = menuBar()->addMenu( "&Scenario" );
	scenarioMenu->addAction( "&Evaluate Scenario", this, &SconeStudio::evaluateActiveScenario, QKeySequence( "Ctrl+E" ) );
	scenarioMenu->addSeparator();
	scenarioMenu->addAction( "&Optimize Scenario", this, &SconeStudio::optimizeScenario, QKeySequence( "Ctrl+F5" ) );
	scenarioMenu->addAction( "Run &Multiple Optimizations", this, &SconeStudio::optimizeScenarioMultiple, QKeySequence( "Ctrl+Shift+F5" ) );
	scenarioMenu->addSeparator();
	scenarioMenu->addAction( "&Abort Optimizations", this, &SconeStudio::abortOptimizations, QKeySequence() );
	scenarioMenu->addSeparator();
	scenarioMenu->addAction( "&Performance Test", this, &SconeStudio::performanceTestNormal, QKeySequence( "Ctrl+P" ) );
	scenarioMenu->addAction( "Performance Test (Write Stats)", this, &SconeStudio::performanceTestWriteStats, QKeySequence( "Ctrl+Shift+P" ) );

	auto toolsMenu = menuBar()->addMenu( "&Tools" );
	toolsMenu->addAction( "Generate &Video...", this, &SconeStudio::createVideo );
	toolsMenu->addAction( "Save &Image...", this, &SconeStudio::captureImage, QKeySequence( "Ctrl+I" ) );
	toolsMenu->addSeparator();
	toolsMenu->addAction( "&Model Analysis", this, &SconeStudio::modelAnalysis );
	toolsMenu->addAction( "M&uscle Analysis", this, &SconeStudio::muscleAnalysis );
	toolsMenu->addAction( "&Gait Analysis", this, &SconeStudio::updateGaitAnalysis, QKeySequence( "Ctrl+G" ) );
	toolsMenu->addAction( "&Keep Current Analysis Graphs", analysisView, &QDataAnalysisView::holdSeries, QKeySequence( "Ctrl+Shift+K" ) );
	toolsMenu->addSeparator();
	toolsMenu->addAction( "&Preferences...", this, &SconeStudio::showSettingsDialog, QKeySequence( "Ctrl+," ) );

	auto* actionMenu = menuBar()->addMenu( "&Playback" );
	actionMenu->addAction( "&Play or Evaluate", ui.playControl, &QPlayControl::togglePlay, Qt::Key_F5 );
	actionMenu->addAction( "&Stop / Reset", ui.playControl, &QPlayControl::stopReset, Qt::Key_F8 );
	actionMenu->addAction( "Toggle Play", ui.playControl, &QPlayControl::togglePlay, QKeySequence( "Ctrl+Space" ) );
	actionMenu->addAction( "Toggle Loop", ui.playControl, &QPlayControl::toggleLoop, QKeySequence( "Ctrl+L" ) );
	actionMenu->addAction( "Play F&aster", ui.playControl, &QPlayControl::faster, QKeySequence( "Ctrl+Up" ) );
	actionMenu->addAction( "Play S&lower", ui.playControl, &QPlayControl::slower, QKeySequence( "Ctrl+Down" ) );
	actionMenu->addSeparator();
	actionMenu->addAction( "Step &Back", ui.playControl, &QPlayControl::stepBack, QKeySequence( "Ctrl+Left" ) );
	actionMenu->addAction( "Step &Forward", ui.playControl, &QPlayControl::stepForward, QKeySequence( "Ctrl+Right" ) );
	actionMenu->addAction( "Page &Back", ui.playControl, &QPlayControl::pageBack, QKeySequence( "Ctrl+PgUp" ) );
	actionMenu->addAction( "Page &Forward", ui.playControl, &QPlayControl::pageForward, QKeySequence( "Ctrl+PgDown" ) );
	actionMenu->addAction( "Goto &Begin", ui.playControl, &QPlayControl::reset, QKeySequence( "Ctrl+Home" ) );
	actionMenu->addAction( "Go to &End", ui.playControl, &QPlayControl::end, QKeySequence( "Ctrl+End" ) );

	createWindowMenu();
	createHelpMenu();

	ui.stackedWidget->setCurrentIndex( 0 );
	ui.playControl->setDigits( 6, 3 );

	// docking
	setDockNestingEnabled( true );
	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::BottomDockWidgetArea );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	registerDockWidget( ui.resultsDock, "&Optimization Results" );

	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
	registerDockWidget( ui.messagesDock, "&Messages" );

	auto* analysis_dock = createDockWidget( "&Analysis", analysisView, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, analysis_dock );

	// gait analysis
	gaitAnalysis = new GaitAnalysis( this );
	gaitAnalysisDock = createDockWidget( "&Gait Analysis", gaitAnalysis, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, gaitAnalysisDock );
	gaitAnalysisDock->hide();

	// parameter view
	parView = new QTableView( this );
	parModel = new ParTableModel();
	parView->setModel( parModel );
	for ( int i = 0; i < parView->horizontalHeader()->count(); ++i )
		parView->horizontalHeader()->setSectionResizeMode( i, i == 0 ? QHeaderView::Stretch : QHeaderView::ResizeToContents );
	parView->verticalHeader()->setSectionResizeMode( QHeaderView::Fixed );
	parView->verticalHeader()->setDefaultSectionSize( 24 );
	parViewDock = createDockWidget( "Optimization &Parameters", parView, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, parViewDock );
	parViewDock->hide();

	//// dof editor
	//dofSliderGroup = new QFormGroup( this );
	//auto* ddw = createDockWidget( "&State", dofSliderGroup, Qt::BottomDockWidgetArea );
	//tabifyDockWidget( analysis_dock, ddw );

	// init scene
	ui.osgViewer->setClearColor( vis::to_osg( scone::GetStudioSetting< xo::color >( "viewer.background" ) ) );
}

bool SconeStudio::init()
{
	// init file model and browser widget
	auto results_folder = scone::GetFolder( SCONE_RESULTS_FOLDER );
	xo::create_directories( results_folder );

	resultsModel = new ResultsFileSystemModel( nullptr );
	ui.resultsBrowser->setModel( resultsModel );
	ui.resultsBrowser->setNumColumns( 1 );
	ui.resultsBrowser->setRoot( to_qt( results_folder ), "*.par;*.sto" );
	ui.resultsBrowser->header()->setFrameStyle( QFrame::NoFrame | QFrame::Plain );

	connect( ui.resultsBrowser->selectionModel(),
		SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&, const QModelIndex& ) ) );

	ui.osgViewer->setScene( &vis::osg_group( scene_.node_id() ) );
	ui.osgViewer->createHud( GetFolder( SCONE_UI_RESOURCE_FOLDER ) / "scone_hud.png" );
	//ui.tabWidget->tabBar()->tabButton( 0, QTabBar::RightSide )->resize( 0, 0 );

	ui.playControl->setRange( 0, 100 );
	connect( ui.playControl, &QPlayControl::playTriggered, this, &SconeStudio::start );
	connect( ui.playControl, &QPlayControl::stopTriggered, this, &SconeStudio::stop );
	connect( ui.playControl, &QPlayControl::timeChanged, this, &SconeStudio::setPlaybackTime );
	connect( ui.playControl, &QPlayControl::sliderReleased, this, &SconeStudio::refreshAnalysis );
	connect( analysisView, &QDataAnalysisView::timeChanged, ui.playControl, &QPlayControl::setTimeStop );

	// start timer for viewer
	connect( &backgroundUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateBackgroundTimer() ) );
	backgroundUpdateTimer.start( 500 );

	// add outputText to global sinks (only *after* the ui has been initialized)
	xo::log::add_sink( ui.outputText );
	ui.outputText->set_sink_mode( xo::log::sink_mode::current_thread );
	ui.outputText->set_log_level( XO_IS_DEBUG_BUILD ? xo::log::level::trace : xo::log::level::debug );

	restoreSettings( "SCONE", "SconeStudio" );
	ui.messagesDock->raise();

	return true;
}

SconeStudio::~SconeStudio()
{}

void SconeStudio::restoreCustomSettings( QSettings& settings )
{
	if ( settings.contains( "viewSettings" ) )
	{
		ModelVis::ViewSettings f( settings.value( "viewSettings" ).toULongLong() );
		for ( auto& va : viewActions )
			va.second->setChecked( f.get( va.first ) );
	}
}

void SconeStudio::saveCustomSettings( QSettings& settings )
{
	ModelVis::ViewSettings f;
	for ( auto& va : viewActions )
		f.set( va.first, va.second->isChecked() );
	settings.setValue( "viewSettings", QVariant( uint( f.data() ) ) );
}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	auto info = ui.resultsBrowser->fileSystemModel()->fileInfo( idx );
	if ( !info.isDir() )
	{
		ui.playControl->reset();
		if ( createScenario( info.absoluteFilePath() ) )
		{
			if ( scenario_->IsEvaluating() ) // .par or .sto
				evaluate();

			ui.playControl->setRange( 0, scenario_->GetMaxTime() );
			ui.playControl->play(); // automatic playback after evaluation

			if ( !gaitAnalysisDock->visibleRegion().isEmpty() )
				updateGaitAnalysis(); // automatic gait analysis if visible
		}
	}
}

void SconeStudio::selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold )
{
	auto item = ui.resultsBrowser->fileSystemModel()->fileInfo( idx );
	string dirname = item.isDir() ? item.filePath().toStdString() : item.dir().path().toStdString();
}

void SconeStudio::start()
{
	auto s = getActiveScenario();
	if ( s && ( !scenario_ ||
		s->document()->isModified() ||
		scenario_->IsEvaluating() ||
		( s->hasFocus() && scenario_->GetFileName() != path_from_qt( s->fileName ) ) ) )
	{
		// update the simulation
		evaluateActiveScenario();
	}
	else
	{
		// everything is up-to-date, pause idle update timer
		ui.osgViewer->stopTimer();
	}
}

void SconeStudio::stop()
{
	ui.osgViewer->startTimer();
	refreshAnalysis();
}

void SconeStudio::refreshAnalysis()
{
	analysisView->refresh( current_time );
}

void SconeStudio::evaluate()
{
	SCONE_ASSERT( scenario_ );

	QProgressDialog dlg( ( "Evaluating " + scenario_->GetFileName().str() ).c_str(), "Abort", 0, 1000, this );
	dlg.setWindowModality( Qt::WindowModal );
	dlg.show();
	QApplication::processEvents();

	xo::set_thread_priority( xo::thread_priority::highest );

	const double step_size = 0.01;
	const xo::time visual_update = 250_ms;
	xo::time prev_visual_time = xo::time() - visual_update;
	xo::timer real_time;
	for ( double t = step_size; scenario_->IsEvaluating(); t += step_size )
	{
		auto rt = real_time();
		if ( rt - prev_visual_time >= visual_update )
		{
			// update 3D visuals and progress bar
			setTime( t, true );
			dlg.setValue( int( 1000 * t / scenario_->GetMaxTime() ) );
			if ( dlg.wasCanceled() )
			{
				// user pressed cancel: update data so that user can see results so far
				scenario_->AbortEvaluation();
				break;
			}
			prev_visual_time = rt;
		}
		else setTime( t, false );
	}

	// report duration
	if ( scenario_->IsReady() )
	{
		auto real_dur = real_time().seconds();
		auto sim_time = scenario_->GetTime();
		log::info( "Evaluation took ", real_dur, "s for ", sim_time, "s (", sim_time / real_dur, "x real-time)" );
	}

	xo::set_thread_priority( xo::thread_priority::normal );

	dlg.setValue( 1000 );
	scenario_->UpdateVis( scenario_->GetTime() );
}

void SconeStudio::createVideo()
{
	if ( !scenario_ )
		return error( "No Scenario", "There is no scenario open" );

	if ( auto p = GetStudioSetting<path>( "video.path_to_ffmpeg" ); !xo::file_exists( p ) )
		return error( "Could not find ffmpeg", to_qt( "Could not find " + p.str() ) );

	if ( ui.viewerDock->isFloating() ) {
		auto borderSize = ui.viewerDock->size() - ui.osgViewer->size();
		auto videoSize = QSize( GetStudioSettings().get<int>( "video.width" ), GetStudioSettings().get<int>( "video.height" ) );
		ui.viewerDock->resize( borderSize + videoSize + QSize( 2, 2 ) );
	}

	captureFilename = QFileDialog::getSaveFileName( this, "Video Filename", QString(), "mp4 files (*.mp4);;avi files (*.avi);;mov files (*.mov)" );
	if ( captureFilename.isEmpty() )
		return;

	// start recording
	QDir().mkdir( captureFilename + ".images" );
	ui.osgViewer->startCapture( captureFilename.toStdString() + ".images/image" );

	ui.osgViewer->stopTimer();
	ui.abortButton->setChecked( false );
	ui.progressBar->setValue( 0 );
	ui.progressBar->setFormat( " Creating Video (%p%)" );
	ui.stackedWidget->setCurrentIndex( 1 );

	const double step_size = ui.playControl->slowMotionFactor() / GetStudioSettings().get<double>( "video.frame_rate" );
	for ( double t = 0.0; t <= scenario_->GetMaxTime(); t += step_size )
	{
		setTime( t, true );
		ui.progressBar->setValue( int( t / scenario_->GetMaxTime() * 100 ) );
		QApplication::processEvents();
		if ( ui.abortButton->isChecked() )
			break;
	}

	// finalize recording
	finalizeCapture();
	ui.stackedWidget->setCurrentIndex( 0 );
	ui.osgViewer->startTimer();
}

void SconeStudio::captureImage()
{
	QString filename = QFileDialog::getSaveFileName( this, "Image Filename", QString(), "png files (*.png)" );
	if ( !filename.isEmpty() )
		ui.osgViewer->captureCurrentFrame( filename.toStdString() );
}

void SconeStudio::modelAnalysis()
{
	if ( scenario_ && scenario_->HasModel() )
		xo::log_prop_node( scenario_->GetModel().GetInfo() );
}

void SconeStudio::muscleAnalysis()
{
	if ( scenario_ && scenario_->HasModel() )
		scone::WriteMuscleInfo( scenario_->GetModel() );
}

void SconeStudio::updateGaitAnalysis()
{
	if ( scenario_ && !scenario_->IsEvaluating() )
	{
		gaitAnalysis->update( scenario_->GetData(), scenario_->GetFileName() );
		gaitAnalysisDock->setWindowTitle( gaitAnalysis->info() );
		gaitAnalysisDock->show();
		gaitAnalysisDock->raise();
	}
}

void SconeStudio::setTime( TimeInSeconds t, bool update_vis )
{
	if ( scenario_ )
	{
		// update current time and stop when done
		current_time = t;

		// update ui and visualization
		if ( scenario_->IsEvaluating() )
			scenario_->EvaluateTo( t );

		if ( update_vis && scenario_->HasModel() )
		{
			scenario_->UpdateVis( t );
			auto d = com_delta( scenario_->GetFollowPoint() );
			ui.osgViewer->moveCamera( osg::Vec3( d.x, d.y, d.z ) );
			ui.osgViewer->setFrameTime( current_time );

			if ( analysisView->isVisible() ) // #todo: not update so much when not playing (it's slow)
				analysisView->refresh( current_time, !ui.playControl->isPlaying() );
		}
	}
}

void SconeStudio::fileOpenTriggered()
{
	QString default_path = to_qt( GetFolder( SCONE_SCENARIO_FOLDER ) );
	if ( auto* s = getActiveScenario() )
		default_path = to_qt( path( s->fileName.toStdString() ).parent_path() );

	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", default_path, "Supported file formats (*.scone *.xml *.zml *.lua *.hfd *.osim);;SCONE Scenarios (*.scone *.xml *.zml);;Lua Scripts (*.lua)" );
	if ( !filename.isEmpty() )
		openFile( filename );
}

void SconeStudio::openFile( const QString& filename )
{
	try
	{
		QCodeEditor* edw = new QCodeEditor( this );
		edw->open( filename );
		edw->setFocus();
		int idx = ui.tabWidget->addTab( edw, edw->getTitle() );
		ui.tabWidget->setCurrentIndex( idx );
		connect( edw, &QCodeEditor::textChanged, this, &SconeStudio::updateTabTitles );
		codeEditors.push_back( edw );
		updateRecentFilesMenu( filename );
		createAndVerifyActiveScenario( false );
	}
	catch ( std::exception& e ) { error( "Error opening " + filename, e.what() ); }
}

void SconeStudio::fileSaveTriggered()
{
	if ( auto* s = getActiveCodeEditor() )
	{
		s->save();
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
		createAndVerifyActiveScenario( true );
	}
}

void SconeStudio::fileSaveAsTriggered()
{
	try
	{
		if ( auto* s = getActiveCodeEditor() )
		{
			// apparently, the mess below is needed to setup the (trivially) correct file filter in Qt
			QString scone_file = "SCONE scenario (*.scone *.xml)";
			QString lua_file = "Lua script (*.lua)";
			QString ext = QFileInfo( s->fileName ).suffix();
			QString filter = scone_file + ";;" + lua_file;
			QString* default_filter = nullptr;
			if ( ext == "scone" ) default_filter = &scone_file;
			else if ( ext == "lua" ) default_filter = &lua_file;

			// we can finally make the actual call
			QString filename = QFileDialog::getSaveFileName( this, "Save File As", s->fileName, filter, default_filter );
			if ( !filename.isEmpty() )
			{
				s->saveAs( filename );
				ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
				updateRecentFilesMenu( s->fileName );
				createAndVerifyActiveScenario( true );
			}
		}
	}
	catch ( std::exception& e ) { error( "Error saving file", e.what() ); }
}

void SconeStudio::fileCloseTriggered()
{
	if ( auto idx = ui.tabWidget->currentIndex(); idx >= 0 )
		tabCloseRequested( idx );
}

bool SconeStudio::tryExit()
{
	if ( abortOptimizations() )
	{
		// wait until all optimizations are actually done
		while ( !optimizations.empty() )
			updateOptimizations();
		return true;
	}
	else return false;
}

void SconeStudio::addProgressDock( ProgressDockWidget* pdw )
{
	optimizations.push_back( pdw );
	if ( optimizations.size() < 4 )
	{
		// stack below results
		addDockWidget( Qt::LeftDockWidgetArea, pdw );
		for ( index_t r = 0; r < optimizations.size(); ++r )
			splitDockWidget( r == 0 ? ui.resultsDock : optimizations[ r - 1 ], optimizations[ r ], Qt::Vertical );
	}
	else
	{
		// organize into columns
		addDockWidget( Qt::LeftDockWidgetArea, pdw );

		auto columns = std::max<int>( 1, ( optimizations.size() + 5 ) / 6 );
		auto rows = ( optimizations.size() + columns - 1 ) / columns;
		log::debug( "Reorganizing windows, columns=", columns, " rows=", rows );

		// first column
		splitDockWidget( optimizations[ 0 ], ui.resultsDock, Qt::Horizontal );
		for ( index_t r = 1; r < rows; ++r )
			splitDockWidget( optimizations[ ( r - 1 ) * columns ], optimizations[ r * columns ], Qt::Vertical );

		// remaining columns
		for ( index_t c = 1; c < columns; ++c )
		{
			for ( index_t r = 0; r < rows; ++r )
			{
				index_t i = r * columns + c;
				if ( i < optimizations.size() )
					splitDockWidget( optimizations[ i - 1 ], optimizations[ i ], Qt::Horizontal );
			}
		}
	}
}

bool SconeStudio::createScenario( const QString& any_file )
{
	scenario_.reset();
	analysisStorageModel.setStorage( nullptr );
	parModel->setObjectiveInfo( nullptr );

	try
	{
		// create scenario and update viewer
		scenario_ = std::make_unique< StudioModel >( scene_, path_from_qt( any_file ) );
		updateViewSettings();

		// update analysis and parview
		analysisStorageModel.setStorage( &scenario_->GetData() );
		analysisView->reset();
		parModel->setObjectiveInfo( &scenario_->GetOjective().info() );
	}
	catch ( std::exception& e )
	{
		error( "Error loading scenario", e.what() );
		scenario_.reset();
		return false;
	}

	// always do this, also in case of error
	ui.playControl->reset();
	ui.playControl->setRange( 0, 0 );
	ui.osgViewer->repaint();

	return scenario_->IsValid();
}

std::vector<QCodeEditor*> SconeStudio::changedDocuments()
{
	std::vector< QCodeEditor* > modified_docs;
	for ( auto s : codeEditors )
		if ( s->document()->isModified() )
			modified_docs.push_back( s );
	return modified_docs;
}

bool SconeStudio::requestSaveChanges( const std::vector<QCodeEditor*>& modified_docs )
{
	if ( !modified_docs.empty() )
	{
		QString message = "The following files have been modified:\n";
		for ( auto s : modified_docs )
			message += "\n" + s->getTitle();

		if ( QMessageBox::warning( this, "Save Changes", message, QMessageBox::Save, QMessageBox::Cancel ) == QMessageBox::Save )
		{
			for ( auto s : modified_docs )
			{
				s->save();
				ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
			}
			return true;
		}
		else return false;
	}
	else return true;
}

bool SconeStudio::requestSaveChanges( QCodeEditor* s )
{
	if ( s && s->document()->isModified() )
	{
		QString message = "Save changes to " + s->getTitle() + "?";
		if ( QMessageBox::warning( this, "Save Changes", message, QMessageBox::Save, QMessageBox::Discard ) == QMessageBox::Save )
			s->save();
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
		return true;
	}
	else return false;
}

int SconeStudio::getTabIndex( QCodeEditor* s )
{
	for ( int idx = 0; idx < ui.tabWidget->count(); ++idx )
		if ( ui.tabWidget->widget( idx ) == (QWidget*)s )
			return idx;
	return -1;
}

QCodeEditor* SconeStudio::getActiveCodeEditor()
{
	for ( auto s : codeEditors )
		if ( !s->visibleRegion().isEmpty() )
			return s;
	return nullptr;
}

QCodeEditor* SconeStudio::getActiveScenario()
{
	QCodeEditor* first_scenario = nullptr;
	for ( auto s : codeEditors )
	{
		auto ext = path_from_qt( s->fileName ).extension_no_dot().str();
		if ( ext == "scone" )
		{
			if ( !s->visibleRegion().isEmpty() && !s->document()->find( "Optimizer" ).isNull() )
				return s; // active scone file
			else if ( first_scenario == nullptr )
				first_scenario = s; // could be single .scone file
		}
	}
	return first_scenario; // either first .scone file, or none
}

bool SconeStudio::createAndVerifyActiveScenario( bool always_create )
{
	if ( auto* s = getActiveScenario() )
	{
		auto changed_docs = changedDocuments();
		if ( !requestSaveChanges( changed_docs ) )
			return false;

		if ( scenario_
			&& scenario_->GetScenarioFileName() == s->fileName
			&& ( scenario_->IsEvaluating() && scenario_->GetTime() == 0.0 )
			&& changed_docs.empty()
			&& !always_create )
			return true; // we already have a scenario

		if ( createScenario( s->fileName ) )
		{
			if ( LogUnusedProperties( scenario_->GetScenarioProps() ) )
			{
				QString message = "Invalid scenario settings in " + scenario_->GetScenarioFileName() + ":\n\n";
				message += to_qt( to_str_unaccessed( scenario_->GetScenarioProps() ) );
				if ( QMessageBox::warning( this, "Invalid scenario settings", message, QMessageBox::Ignore, QMessageBox::Cancel ) == QMessageBox::Cancel )
					return false; // user pressed cancel
			}
			return true; // everything loaded ok or invalid settings ignored
		}
		else return false; // failed to create scenario
	}
	else
	{
		information( "No Scenario Selected", "Please select a .scone file" );
		return false;
	}
}

void SconeStudio::optimizeScenario()
{
	try
	{
		if ( createAndVerifyActiveScenario( true ) )
		{
			auto task = scone::createOptimizerTask( scenario_->GetScenarioFileName() );
			addProgressDock( new ProgressDockWidget( this, std::move( task ) ) );
			updateOptimizations();
		}

	}
	catch ( const std::exception& e )
	{
		error( "Error optimizing " + scenario_->GetScenarioFileName(), e.what() );
	}
}

void SconeStudio::optimizeScenarioMultiple()
{
	try
	{
		if ( createAndVerifyActiveScenario( true ) )
		{
			bool ok = true;
			int count = QInputDialog::getInt( this, "Run Multiple Optimizations", "Enter number of optimization instances: ", 3, 1, 100, 1, &ok );
			if ( ok )
			{
				for ( int i = 1; i <= count; ++i )
				{
					QStringList args( QString().sprintf( "#1.random_seed=%d", i ) );
					auto task = createOptimizerTask( scenario_->GetScenarioFileName(), args );
					addProgressDock( new ProgressDockWidget( this, std::move( task ) ) );
					QApplication::processEvents(); // needed for the ProgressDockWidgets to be evenly sized
				}
				updateOptimizations();
			}
		}
	}
	catch ( const std::exception& e )
	{
		error( "Error optimizing " + scenario_->GetScenarioFileName(), e.what() );
	}
}

void SconeStudio::evaluateActiveScenario()
{
	if ( createAndVerifyActiveScenario( false ) )
	{
		if ( scenario_->IsEvaluating() )
			evaluate();
		ui.playControl->setRange( 0, scenario_->GetMaxTime() );
		ui.playControl->play();
	}
}

void SconeStudio::performanceTest( bool write_stats )
{
	if ( createAndVerifyActiveScenario( false ) )
	{
		auto par = SearchPoint( scenario_->GetModelObjective().info() );
		xo::timer real_time;
		auto model = scenario_->GetModelObjective().CreateModelFromParams( par );
		model->SetStoreData( false );
		model->AdvanceSimulationTo( model->GetSimulationEndTime() );
		auto real_dur = real_time().seconds();
		auto sim_time = model->GetTime();
		if ( model->GetProfiler().enabled() )
			model->GetProfiler().log_results();

		log::info( "fitness = ", scenario_->GetModelObjective().GetResult( *model ) );
		if ( auto sim_report = model->GetSimulationReport(); !sim_report.empty() )
			log::info( sim_report );
		if ( write_stats )
			model->UpdatePerformanceStats( scenario_->GetFileName() );
		log::info( "Evaluation took ", real_dur, "s for ", sim_time, "s (", sim_time / real_dur, "x real-time)" );
	}
}

bool SconeStudio::abortOptimizations()
{
	if ( optimizations.size() > 0 )
	{
		bool showWarning = false;
		QString message = QString().sprintf( "Are you sure you want to abort the following optimizations:\n\n" );
		for ( auto& o : optimizations )
		{
			showWarning |= !o->canCloseWithoutWarning();
			message += o->getIdentifier() + "\n";
		}

		if ( !showWarning || QMessageBox::warning( this, "Abort Optimizations", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Abort )
		{
			for ( const auto& o : optimizations )
			{
				o->disableCloseWarning();
				o->close();
			}
			return true;
		}
		else return false;
	}
	else return true;
}

void SconeStudio::updateBackgroundTimer()
{
	updateOptimizations();
}

void SconeStudio::updateOptimizations()
{
	// clear out all closed optimizations
	for ( auto it = optimizations.begin(); it != optimizations.end(); )
	{
		ProgressDockWidget* w = *it;
		if ( w->readyForDestruction() )
		{
			delete w;
			it = optimizations.erase( it );
		}
		else ++it;
	}

	// update all optimizations
	for ( auto& o : optimizations )
	{
		if ( o->updateProgress() == ProgressDockWidget::ShowErrorResult )
		{
			QString title = "Error optimizing " + o->task_->scenario_file_;
			QString msg = o->message.c_str();
			o->close();
			QMessageBox::critical( this, title, msg );
			return; // must return here because close invalidates the iterator
		}
	}
}

void SconeStudio::tabCloseRequested( int idx )
{
	auto it = xo::find( codeEditors, (QCodeEditor*)ui.tabWidget->widget( idx ) );
	SCONE_THROW_IF( it == codeEditors.end(), "Could not find scenario for tab " + to_str( idx ) );

	requestSaveChanges( *it );
	codeEditors.erase( it );
	ui.tabWidget->removeTab( idx );
}

void SconeStudio::updateViewSettings()
{
	if ( scenario_ )
	{
		ModelVis::ViewSettings f;
		for ( auto& va : viewActions )
			f.set( va.first, va.second->isChecked() );
		scenario_->ApplyViewSettings( f );
		ui.osgViewer->repaint();
	}
}

void SconeStudio::updateTabTitles()
{
	for ( auto s : codeEditors )
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
}

void SconeStudio::finalizeCapture()
{
	ui.osgViewer->stopCapture();

	const auto ffmpeg = GetStudioSetting<path>( "video.path_to_ffmpeg" );
	if ( !xo::file_exists( ffmpeg ) )
		return error( "Could not find ffmpeg", to_qt( "Could not find " + ffmpeg.str() ) );

	QString program = to_qt( ffmpeg );
	QStringList args;
	args << "-r" << to_qt( GetStudioSettings().get<string>( "video.frame_rate" ) )
		<< "-i" << captureFilename + ".images/image_0_%d.png"
		<< "-c:v" << "mpeg4"
		<< "-q:v" << to_qt( GetStudioSettings().get<string>( "video.quality" ) )
		<< captureFilename;

	std::cout << "starting " << program.toStdString() << endl;
	auto v = args.toVector();
	for ( auto arg : v )
		std::cout << arg.toStdString() << endl;

	captureProcess = new QProcess( this );
	captureProcess->start( program, args );

	xo_error_if( !captureProcess->waitForStarted( 5000 ), "Could not start process" );
	scone::log::info( "Generating video for ", captureFilename.toStdString() );

	if ( !captureProcess->waitForFinished( 30000 ) )
		scone::log::error( "Did not finish in time" );

	scone::log::info( "Video generated" );
	QDir( captureFilename + ".images" ).removeRecursively();

	delete captureProcess;
	captureProcess = nullptr;
	captureFilename.clear();
}
