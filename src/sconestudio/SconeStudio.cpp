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

using namespace scone;
using namespace xo::literals;

SconeStudio::SconeStudio( QWidget* parent, Qt::WindowFlags flags ) :
	QCompositeMainWindow( parent, flags ),
	close_all( false ),
	current_time(),
	evaluation_time_step( 1.0 / 8 ),
	scene_( true, GetStudioSetting< float >( "viewer.ambient_intensity" ) ),
	slomo_factor( 1 ),
	com_delta( Vec3( 0, 1, 0 ) ),
	captureProcess( nullptr )
{
	xo::log::debug( "Constructing UI elements" );
	ui.setupUi( this );

	createFileMenu( to_qt( GetFolder( SCONE_SCENARIO_FOLDER ) ), "Scone Scenario (*.scone)" );

	auto editMenu = menuBar()->addMenu( "&Edit" );
	addMenuAction( editMenu, "&Find...", this, &SconeStudio::findDialog, QKeySequence( "Ctrl+F" ) );
	addMenuAction( editMenu, "Find &Next", this, &SconeStudio::findNext, Qt::Key_F3 );
	addMenuAction( editMenu, "Find &Previous", this, &SconeStudio::findPrevious, QKeySequence( "Shift+F3" ) );
	editMenu->addSeparator();
	addMenuAction( editMenu, "&Preferences...", this, &SconeStudio::showSettingsDialog );

	auto viewMenu = menuBar()->addMenu( "&View" );
	viewActions[ ModelVis::ShowForces ] = addMenuAction( viewMenu, "Show External &Forces", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowMuscles ] = addMenuAction( viewMenu, "Show &Muscles", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowTendons ] = addMenuAction( viewMenu, "Show &Tendons", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowGeometry ] = addMenuAction( viewMenu, "Show &Bone Geometry", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowJoints ] = addMenuAction( viewMenu, "Show &Joints", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowContactGeom ] = addMenuAction( viewMenu, "Show &Contact Geometry", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowAxes ] = addMenuAction( viewMenu, "Show Body &Axes", this, &SconeStudio::updateViewSettings );
	viewActions[ ModelVis::ShowGroundPlane ] = addMenuAction( viewMenu, "Show &Ground Plane", this, &SconeStudio::updateViewSettings );
	for ( auto& va : viewActions )
	{
		va.second->setCheckable( true );
		va.second->setChecked( va.first != ModelVis::ShowAxes && va.first != ModelVis::ShowJoints );
	}

	auto scenarioMenu = menuBar()->addMenu( "&Scenario" );
	addMenuAction( scenarioMenu, "&Evaluate Scenario", this, &SconeStudio::evaluateActiveScenario, QKeySequence( "Ctrl+E" ) );
	scenarioMenu->addSeparator();
	addMenuAction( scenarioMenu, "&Optimize Scenario", this, &SconeStudio::startOptimization, QKeySequence( "Ctrl+F5" ) );
	addMenuAction( scenarioMenu, "Run &Multiple Optimizations", this, &SconeStudio::startMultipleOptimizations, QKeySequence( "Ctrl+Shift+F5" ));
	scenarioMenu->addSeparator();
	addMenuAction( scenarioMenu, "&Abort Optimizations", this, &SconeStudio::abortOptimizations, QKeySequence() );
	scenarioMenu->addSeparator();
	addMenuAction( scenarioMenu, "&Performance Test (Profile)", this, &SconeStudio::performanceTestProfile, QKeySequence( "Ctrl+P" ) );
	addMenuAction( scenarioMenu, "Performance Test (&Raw)", this, &SconeStudio::performanceTestNoProfile, QKeySequence( "Ctrl+Shift+P" ) );

	auto toolsMenu = menuBar()->addMenu( "&Tools" );
	addMenuAction( toolsMenu, "Generate &Video...", this, &SconeStudio::createVideo );
	addMenuAction( toolsMenu, "Save &Image...", this, &SconeStudio::captureImage, QKeySequence( "Ctrl+I" ) );
	toolsMenu->addSeparator();
	addMenuAction( toolsMenu, "&Model Analysis", this, &SconeStudio::modelAnalysis );
	addMenuAction( toolsMenu, "M&uscle Analysis", this, &SconeStudio::muscleAnalysis );
	addMenuAction( toolsMenu, "&Gait Analysis", this, &SconeStudio::updateGaitAnalysis, QKeySequence( "Ctrl+G" ) );
	toolsMenu->addSeparator();
	addMenuAction( toolsMenu, "&Preferences...", this, &SconeStudio::showSettingsDialog );

	auto* actionMenu = menuBar()->addMenu( "&Playback" );
	addMenuAction( actionMenu, "&Play or Evaluate", ui.playControl, &QPlayControl::togglePlay, Qt::Key_F5 );
	addMenuAction( actionMenu, "&Stop / Reset", ui.playControl, &QPlayControl::stopReset, Qt::Key_F8 );
	addMenuAction( actionMenu, "Toggle Play", ui.playControl, &QPlayControl::togglePlay, QKeySequence( "Ctrl+Space" ) );
	addMenuAction( actionMenu, "Toggle Loop", ui.playControl, &QPlayControl::toggleLoop, QKeySequence( "Ctrl+L" ) );
	addMenuAction( actionMenu, "Play F&aster", ui.playControl, &QPlayControl::faster, QKeySequence( "Ctrl+Up" ) );
	addMenuAction( actionMenu, "Play S&lower", ui.playControl, &QPlayControl::slower, QKeySequence( "Ctrl+Down" ) );
	actionMenu->addSeparator();
	addMenuAction( actionMenu, "Step &Back", ui.playControl, &QPlayControl::stepBack, QKeySequence( "Ctrl+Left" ) );
	addMenuAction( actionMenu, "Step &Forward", ui.playControl, &QPlayControl::stepForward, QKeySequence( "Ctrl+Right" ) );
	addMenuAction( actionMenu, "Page &Back", ui.playControl, &QPlayControl::pageBack, QKeySequence( "Ctrl+PgUp" ) );
	addMenuAction( actionMenu, "Page &Forward", ui.playControl, &QPlayControl::pageForward, QKeySequence( "Ctrl+PgDown" ) );
	addMenuAction( actionMenu, "Goto &Begin", ui.playControl, &QPlayControl::reset, QKeySequence( "Ctrl+Home" ) );
	addMenuAction( actionMenu, "Go to &End", ui.playControl, &QPlayControl::end, QKeySequence( "Ctrl+End" ) );

	createWindowMenu();
	createHelpMenu();

	ui.stackedWidget->setCurrentIndex( 0 );
	ui.playControl->setDigits( 6, 3 );

	// analysis
	analysisView = new QDataAnalysisView( &analysisStorageModel, this );
	analysisView->setObjectName( "Analysis" );
	analysisView->setMinSeriesInterval( 0 );
	analysisView->setLineWidth( scone::GetStudioSettings().get< float >( "analysis.line_width" ) );

	// gait analysis
	gaitAnalysis = new GaitAnalysis( this );

	// docking
	setDockNestingEnabled( true );
	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::BottomDockWidgetArea );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	registerDockWidget( ui.resultsDock, "Optimization &Results" );

	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
	registerDockWidget( ui.messagesDock, "&Messages" );

	auto* analysis_dock = createDockWidget( "&Analysis", analysisView, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, analysis_dock );

	gaitAnalysisDock = createDockWidget( "&Gait Analysis", gaitAnalysis, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, gaitAnalysisDock );
	gaitAnalysisDock->hide();

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
	connect( analysisView, &QDataAnalysisView::timeChanged, ui.playControl, &QPlayControl::setTime );

	// start timer for viewer
	connect( &backgroundUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateBackgroundTimer() ) );
	backgroundUpdateTimer.start( 500 );

	// add outputText to global sinks (only *after* the ui has been initialized)
	xo::log::add_sink( ui.outputText );
	ui.outputText->set_log_level( XO_IS_DEBUG_BUILD ? xo::log::level::trace : xo::log::level::debug );

	restoreSettings( "SCONE", "SconeStudio" );
	ui.messagesDock->raise();

	return true;
}

SconeStudio::~SconeStudio()
{}

void SconeStudio::restoreCustomSettings( QSettings& settings )
{
	if ( settings.contains( "viewSettings"))
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
	settings.setValue( "viewSettings", QVariant(uint(f.data())));
}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	QString browserItem = ui.resultsBrowser->fileSystemModel()->fileInfo( idx ).absoluteFilePath();
	ui.playControl->reset();
	if ( createScenario( browserItem ) )
	{
		if ( scenario_->IsEvaluating() ) // .par or .sto
			evaluate();

		ui.playControl->setRange( 0, scenario_->GetMaxTime() );
		ui.playControl->play(); // automatic playback after evaluation

		if ( !gaitAnalysisDock->visibleRegion().isEmpty() )
			updateGaitAnalysis(); // automatic gait analysis if visible
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
		// everything is up-to-date, pause idle update timer and focus viewer
		ui.osgViewer->stopTimer();
		ui.osgViewer->viewWidget()->setFocus();
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
			if ( dlg.wasCanceled() ) {
				scenario_->FinalizeEvaluation( false );
				break;
			}
			prev_visual_time = rt;
		}
		else setTime( t, false );
	}

	// report duration
	auto real_dur = real_time().seconds();
	auto sim_time = scenario_->GetTime();
	log::info( "Evaluation took ", real_dur, "s for ", sim_time, "s (", sim_time / real_dur, "x real-time)" );

	xo::set_thread_priority( xo::thread_priority::normal );

	dlg.setValue( 1000 );
	scenario_->UpdateVis( scenario_->GetTime() );
}

void SconeStudio::createVideo()
{
	if ( !scenario_ )
		return error( "No Scenario", "There is no scenario open" );

	if ( ui.viewerDock->isFloating() ) {
		auto borderSize = ui.viewerDock->size() - ui.osgViewer->size();
		auto videoSize = QSize( GetStudioSettings().get<int>( "video.width" ), GetStudioSettings().get<int>( "video.height" ) );
		ui.viewerDock->resize( borderSize + videoSize + QSize( 2, 2 ) );
	}

	captureFilename = QFileDialog::getSaveFileName( this, "Video Filename", QString(), "avi files (*.avi)" );
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
			auto d = com_delta( scenario_->GetModel().GetComPos() );
			ui.osgViewer->moveCamera( osg::Vec3( d.x, 0, d.z ) );
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
	catch ( std::exception & e ) { error( "Error opening " + filename, e.what() ); }
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
			QString filename = QFileDialog::getSaveFileName( this, "Save File As", s->fileName, "SCONE file (*.scone);;XML file (*.xml);;Lua script (*.lua)" );
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
	abortOptimizations();
	return optimizations.empty();
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

	try
	{
		// create scenario and update viewer
		scenario_ = std::make_unique< StudioModel >( scene_, path_from_qt( any_file ) );
		updateViewSettings();

		// update analysis
		analysisStorageModel.setStorage( &scenario_->GetData() );
		analysisView->reset();

	}
	catch ( std::exception & e )
	{
		error( "Error loading scenario", e.what() );
	}

	// always do this, also in case of error
	ui.playControl->reset();
	ui.playControl->setRange( 0, 0 );
	ui.osgViewer->repaint();

	return bool( scenario_ );
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
			if ( !s->visibleRegion().isEmpty() )
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
			false;

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

void SconeStudio::startOptimization()
{
	if ( createAndVerifyActiveScenario( true ) )
	{
		ProgressDockWidget* pdw = new ProgressDockWidget( this, scenario_->GetScenarioFileName() );
		addProgressDock( pdw );
		updateOptimizations();
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

void SconeStudio::performanceTest( bool profile )
{
	ui.playControl->stop();
	if ( createAndVerifyActiveScenario( false ) )
	{
		xo::timer real_time;
		if ( profile )
			SCONE_PROFILE_START;
		auto& model = scenario_->GetModel();
		model.SetStoreData( false );
		model.AdvanceSimulationTo( model.GetSimulationEndTime() );
		auto real_dur = real_time().seconds();
		auto sim_time = model.GetTime();
		if ( profile )
			SCONE_PROFILE_REPORT;
		log::info( "fitness = ", scenario_->GetModelObjective().GetResult( model ) );
		if ( auto sim_report = model.GetSimulationReport(); !sim_report.empty() )
			log::info( sim_report );
		log::info( "Evaluation took ", real_dur, "s for ", sim_time, "s (", sim_time / real_dur, "x real-time)" );
	}
}

void SconeStudio::startMultipleOptimizations()
{
	if ( createAndVerifyActiveScenario( true ) )
	{
		bool ok = true;
		int count = QInputDialog::getInt( this, "Run Multiple Optimizations", "Enter number of optimization instances: ", 3, 1, 100, 1, &ok );
		if ( ok )
		{
			for ( int i = 1; i <= count; ++i )
			{
				QStringList args;
				args << QString().sprintf( "#1.random_seed=%d", i );
				ProgressDockWidget* pdw = new ProgressDockWidget( this, scenario_->GetScenarioFileName(), args );
				addProgressDock( pdw );
			}
			updateOptimizations();
		}
	}
}

void SconeStudio::abortOptimizations()
{
	if ( optimizations.size() > 0 )
	{
		QString message = QString().sprintf( "Are you sure you want to abort the following optimizations:\n\n", optimizations.size() );

		for ( auto& o : optimizations )
			message += o->getIdentifier() + "\n";

		if ( QMessageBox::warning( this, "Abort Optimizations", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Abort )
		{
			close_all = true;
			for ( auto& o : optimizations )
				o->close();
			optimizations.clear();
			close_all = false;
		}
	}
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
			QString title = to_qt( "Error optimizing " + o->fileName );
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

	QString program = to_qt( xo::get_application_dir() / SCONE_FFMPEG_EXECUTABLE );
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
