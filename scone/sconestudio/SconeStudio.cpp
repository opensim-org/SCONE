#include "SconeStudio.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include <osgDB/ReadFile>

#include <QtWidgets/QFileSystemModel.h>
#include <QtWidgets/QMessageBox.h>
#include <QtWidgets/QFileDialog>
#include <QTextStream>

#include "simvis/osg_tools.h"
#include "scone/optimization/opt_tools.h"
#include "flut/system_tools.hpp"
#include "qt_tools.h"
#include "qevent.h"
#include "qcustomplot.h"
#include "studio_config.h"
#include "ui_SconeSettings.h"
#include "scone/core/Profiler.h"
#include "ReflexAnalysisObjective.h"
#include "spot/cma_optimizer.h"
#include "spot/console_reporter.h"
#include "spot/file_reporter.h"
#include "flut/filesystem.hpp"
#include "simvis/color.h"

using namespace scone;
using namespace std;

SconeStudio::SconeStudio( QWidget *parent, Qt::WindowFlags flags ) :
QCompositeMainWindow( parent, flags ),
slomo_factor( 1 ),
com_delta( Vec3( 0, 1, 0 ) ),
close_all( false ),
capture_frequency( 30 ),
evaluation_time_step( 1.0 / 8 ),
captureProcess( nullptr ),
scene( true )
{
	flut::log::debug( "Constructing UI elements" );
	ui.setupUi( this );
	ui.stackedWidget->setCurrentIndex( 0 );

	analysisView = new QDataAnalysisView( &storageModel, this );
	analysisView->setObjectName( "Analysis" );
	analysisView->setMinSeriesInterval( 0 );

	auto toolsMenu = menuBar()->addMenu( "&Tools" );
	addMenuAction( toolsMenu, "Capture &Video", this, &SconeStudio::createVideo );
	addMenuAction( toolsMenu, "Capture &Image", this, &SconeStudio::captureImage, QKeySequence( "Ctrl+I" ), true );
	addMenuAction( toolsMenu, "Perform &Reflex Analysis", this, &SconeStudio::performReflexAnalysis, QKeySequence(), true );
	addMenuAction( toolsMenu, "&Preferences", this, &SconeStudio::showSettingsDialog );

	// create window menu
	auto* actionMenu = menuBar()->addMenu( "&Playback" );
	addMenuAction( actionMenu, "&Play", ui.playControl, &QPlayControl::togglePlay, Qt::Key_F5 );
	addMenuAction( actionMenu, "&Stop / Reset", ui.playControl, &QPlayControl::stopReset, Qt::Key_F8 );
	addMenuAction( actionMenu, "Toggle Play", ui.playControl, &QPlayControl::togglePlay, Qt::Key_Space );
	addMenuAction( actionMenu, "Toggle Loop", ui.playControl, &QPlayControl::toggleLoop, QKeySequence( "Ctrl+L" ) );
	addMenuAction( actionMenu, "Play F&aster", ui.playControl, &QPlayControl::faster, QKeySequence( "Ctrl+Up" ) );
	addMenuAction( actionMenu, "Play S&lower", ui.playControl, &QPlayControl::slower, QKeySequence( "Ctrl+Down" ), true );
	addMenuAction( actionMenu, "Step &Back", ui.playControl, &QPlayControl::stepBack, QKeySequence( "Ctrl+Left" ) );
	addMenuAction( actionMenu, "Step &Forward", ui.playControl, &QPlayControl::stepForward, QKeySequence( "Ctrl+Right" ) );
	addMenuAction( actionMenu, "Page &Back", ui.playControl, &QPlayControl::pageBack, QKeySequence( "Ctrl+PgUp" ) );
	addMenuAction( actionMenu, "Page &Forward", ui.playControl, &QPlayControl::pageForward, QKeySequence( "Ctrl+PgDown" ) );
	addMenuAction( actionMenu, "Goto &Begin", ui.playControl, &QPlayControl::reset, QKeySequence( "Ctrl+Home" ) );
	addMenuAction( actionMenu, "Go to &End", ui.playControl, &QPlayControl::end, QKeySequence( "Ctrl+End" ), true );
	addMenuAction( actionMenu, "&Test Current Scenario", this, &SconeStudio::runScenario, QKeySequence( "Ctrl+T" ) );

	createWindowMenu();
	createHelpMenu();

	setDockNestingEnabled( true );
	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	registerDockWidget( ui.resultsDock, "Optimization &Results" );
	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
	registerDockWidget( ui.messagesDock, "&Messages" );
	auto* adw = createDockWidget( "&Analysis", analysisView, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, adw );

	// init scene
	scene.add_light( vis::vec3f( -20, 80, 40 ), vis::make_white( 1 ) );
	scene.create_tile_floor( 64, 64, 1 );
}

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
{
	// init file model and browser widget
	resultsModel = new ResultsFileSystemModel( nullptr );
	auto results_folder = make_qt( scone::GetFolder( SCONE_RESULTS_FOLDER ) );
	QDir().mkdir( results_folder );
	ui.resultsBrowser->setModel( resultsModel );
	ui.resultsBrowser->setNumColumns( 1 );
	ui.resultsBrowser->setRoot( results_folder, "*.par" );

	connect( ui.resultsBrowser->selectionModel(),
		SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&, const QModelIndex& ) ) );

	ui.osgViewer->setScene( scene.osg_group().asNode() );
	ui.tabWidget->tabBar()->tabButton( 0, QTabBar::RightSide )->resize( 0, 0 );

	ui.playControl->setRange( 0, 100 );
	connect( ui.playControl, &QPlayControl::playTriggered, this, &SconeStudio::start );
	connect( ui.playControl, &QPlayControl::stopTriggered, this, &SconeStudio::stop );
	connect( ui.playControl, &QPlayControl::timeChanged, this, &SconeStudio::setPlaybackTime );
	//connect( ui.playControl, &QPlayControl::timeChanged, this, &SconeStudio::refreshAnalysis );
	connect( ui.playControl, &QPlayControl::sliderReleased, this, &SconeStudio::refreshAnalysis );
	//connect( ui.playControl, &QPlayControl::nextTriggered, this, &SconeStudio::refreshAnalysis );
	//connect( ui.playControl, &QPlayControl::previousTriggered, this, &SconeStudio::refreshAnalysis );
	connect( analysisView, &QDataAnalysisView::timeChanged, ui.playControl, &QPlayControl::setTime );

	// start timer for viewer
	connect( &backgroundUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateBackgroundTimer() ) );
	backgroundUpdateTimer.start( 1000 );

	// only do this after the ui has been initialized
	flut::log::add_sink( ui.outputText );
	ui.outputText->set_log_level( flut::log::trace_level );

	flut::log::debug( "Loading GUI settings" );
	QSettings cfg( "SCONE", "SconeStudio" );
	restoreGeometry( cfg.value( "geometry" ).toByteArray() );
	restoreState( cfg.value( "windowState" ).toByteArray() );
	recentFiles = cfg.value( "recentFiles" ).toStringList();

	updateRecentFilesMenu();

	ui.messagesDock->raise();

	return true;
}

SconeStudio::~SconeStudio()
{}

void SconeStudio::runSimulation( const QString& filename )
{
	SCONE_PROFILE_RESET;
	if ( createModel( filename.toStdString() ) )
	{
		updateViewSettings();
		storageModel.setStorage( &model->GetData() );
		analysisView->reset();
		if ( model->IsEvaluating() )
			evaluate();
		ui.playControl->setRange( 0, model->GetMaxTime() );
	}
	SCONE_PROFILE_REPORT;
}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	currentParFile = ui.resultsBrowser->fileSystemModel()->fileInfo( idx ).absoluteFilePath();
	showViewer();
	ui.playControl->reset();
	runSimulation( currentParFile );
	if ( model )
		ui.playControl->play();
}

void SconeStudio::selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold )
{
	auto item = ui.resultsBrowser->fileSystemModel()->fileInfo( idx );
	string dirname = item.isDir() ? item.filePath().toStdString() : item.dir().path().toStdString();
}

void SconeStudio::start()
{
	ui.osgViewer->stopTimer();
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
	ui.playControl->reset();
	ui.abortButton->setChecked( false );
	ui.progressBar->setValue( 0 );
	ui.progressBar->setFormat( " Evaluating (%p%)" );
	ui.stackedWidget->setCurrentIndex( 1 );

	const double step_size = 0.05;
	int vis_step = 0;
	flut::timer real_time;
	for ( double t = step_size; t < model->GetMaxTime(); t += step_size )
	{
		ui.progressBar->setValue( int( t / model->GetMaxTime() * 100 ) );
		QApplication::processEvents();
		if ( ui.abortButton->isChecked() )
		{
			model->FinalizeEvaluation( false );
			ui.stackedWidget->setCurrentIndex( 0 );
			return;
		}
		setTime( t, vis_step++ % 5 == 0 );
	}

	// report duration
	auto real_dur = real_time.seconds();
	auto sim_time = model->GetTime();
	log::info( "Evaluation took ", real_dur, "s for ", sim_time, "s (", sim_time / real_dur, "x real-time)" );

	ui.progressBar->setValue( 100 );
	if ( model->IsEvaluating() )
		model->EvaluateTo( model->GetMaxTime() );
	model->UpdateVis( model->GetTime() );

	ui.stackedWidget->setCurrentIndex( 0 );
}

void SconeStudio::createVideo()
{
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

	const double step_size = ui.playControl->slowMotionFactor() / 30.0;
	for ( double t = 0.0; t <= model->GetMaxTime(); t += step_size )
	{
		setTime( t, true );
		ui.progressBar->setValue( int( t / model->GetMaxTime() * 100 ) );
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

void SconeStudio::setTime( TimeInSeconds t, bool update_vis )
{
	if ( model )
	{
		// update current time and stop when done
		current_time = t;

		// update ui and visualization
		if ( model->IsEvaluating() )
			model->EvaluateTo( t );

		if ( update_vis )
		{
			model->UpdateVis( t );
			auto d = com_delta( model->GetSimModel().GetComPos() );
			ui.osgViewer->moveCamera( osg::Vec3( d.x, 0, d.z ) );
			ui.osgViewer->setFrameTime( current_time );
			if ( analysisView->isVisible() )
				analysisView->refresh( current_time, false );
		}
	}
}

void SconeStudio::fileOpen()
{
	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", make_qt( scone::GetFolder( SCONE_SCENARIO_FOLDER ) ), "SCONE Scenarios (*.xml)" );
	if ( !filename.isEmpty() )
		fileOpen( filename );
}

void SconeStudio::fileOpen( const QString& filename )
{
	QCodeEditor* edw = new QCodeEditor( this );
	edw->open( filename );
	int idx = ui.tabWidget->addTab( edw, edw->getTitle() );
	ui.tabWidget->setCurrentIndex( idx );
	connect( edw, &QCodeEditor::textChanged, this, &SconeStudio::updateTabTitles );
	scenarios.push_back( edw );
	addRecentFile( filename );
}

void SconeStudio::fileOpenRecent()
{
	auto act = qobject_cast<QAction*>( sender() );
	fileOpen( act->text() );
}

void SconeStudio::fileSave()
{
	if ( auto* s = getActiveScenario() )
	{
		//log::trace( "Active scenario: ", getActiveScenario()->fileName.toStdString() );
		s->save();
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
	}
}

void SconeStudio::fileSaveAs()
{
	if ( auto* s = getActiveScenario() )
	{
		s->saveAsDialog( make_qt( scone::GetFolder( SCONE_SCENARIO_FOLDER ) ), "SCONE Scenarios (*.xml)" );
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
		addRecentFile( s->fileName );
	}
}

void SconeStudio::fileExit()
{
	log::info( "Exiting SCONE" );
	close();
}

void SconeStudio::addProgressDock( ProgressDockWidget* pdw )
{
	optimizations.push_back( pdw );
	addDockWidget( optimizations.size() < 3 ? Qt::BottomDockWidgetArea : Qt::RightDockWidgetArea, pdw );

	// organize into columns
	if ( optimizations.size() >= 3 )
	{ 
		auto tab_count = std::min<int>( 3, ( optimizations.size() + 3 ) / 4 );
		for ( size_t i = 0; i < optimizations.size(); ++ i )
		{
			addDockWidget( Qt::RightDockWidgetArea, optimizations[ i ] );
			if ( i % tab_count != 0 )
				//splitDockWidget( optimizations[ i / tab_count * tab_count ], optimizations[ i ] );
				splitDockWidget( optimizations[ i - 1 ], optimizations[ i ], Qt::Horizontal );
		}
	}
}

void SconeStudio::addRecentFile( const QString& filename )
{
	recentFiles.push_front( filename );
	recentFiles.removeDuplicates();
	while ( recentFiles.size() > 10 ) recentFiles.removeLast();
	updateRecentFilesMenu();
}

void SconeStudio::updateRecentFilesMenu()
{
	// init recent files menu
	QMenu* recent_menu = new QMenu();
	for ( int idx = 0; idx < recentFiles.size(); ++idx )
	{
		QAction* act = recent_menu->addAction( recentFiles[ idx ] );
		connect( act, SIGNAL( triggered() ), this, SLOT( fileOpenRecent() ) );
	}
	ui.action_Recent->setMenu( recent_menu );
}

bool SconeStudio::createModel( const String& par_file, bool force_evaluation )
{
	try
	{
		model.reset();
		model = StudioModelUP( new StudioModel( scene, path( par_file ), force_evaluation ) );
	}
	catch ( std::exception& e )
	{
		error( "Could not create model", e.what() );
		return false;
	}
	return true;
}

bool SconeStudio::checkAndSaveScenario( QCodeEditor* s )
{
	if ( s == nullptr )
	{
		QMessageBox::information( this, "No Scenario Selected", "No Scenario open for editing" );
		return false;
	}

	if ( s->hasTextChanged() )
	{
		QString message = "Save changes to " + s->getTitle() + "?";
		if ( QMessageBox::warning( this, "Save Changes", message, QMessageBox::Save, QMessageBox::Discard ) == QMessageBox::Save )
			s->save();
	}

	ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );

	return true;
}

int SconeStudio::getTabIndex( QCodeEditor* s )
{
	for ( int idx = 0; idx < ui.tabWidget->count(); ++idx )
	{
		if ( ui.tabWidget->widget( idx ) == (QWidget*)s )
			return idx;
	}
	return -1;
}

void SconeStudio::optimizeScenario()
{
	if ( checkAndSaveScenario( getActiveScenario() ) )
	{
		ProgressDockWidget* pdw = new ProgressDockWidget( this, getActiveScenario()->fileName );
		addProgressDock( pdw );
		updateOptimizations();
	}
}

void SconeStudio::runScenario()
{
	if ( checkAndSaveScenario( getActiveScenario() ) )
		runSimulation( getActiveScenario()->fileName );
}

void SconeStudio::optimizeScenarioMultiple()
{
	if ( checkAndSaveScenario( getActiveScenario() ) )
	{
		bool ok = true;
		int count = QInputDialog::getInt( this, "Run Multiple Optimizations", "Enter number of optimization instances: ", 3, 1, 100, 1, &ok );
		if ( ok )
		{
			for ( int i = 1; i <= count; ++i )
			{
				QStringList args;
				args << QString().sprintf( "Optimizer.random_seed=%d", i );
				ProgressDockWidget* pdw = new ProgressDockWidget( this, getActiveScenario()->fileName, args );
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
			{
				o->close();
			}
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
			QString title = "Error optimizing " + o->fileName;
			QString msg = o->errorMsg;
			o->close();
			QMessageBox::critical( this, title, msg );
			return; // must return here because close invalidates the iterator
		}
	}
}

void SconeStudio::tabCloseRequested( int idx )
{
	SCONE_ASSERT( idx > 0 && idx <= (int)scenarios.size() );
	auto it = scenarios.begin() + ( idx - 1 );

	if ( checkAndSaveScenario( *it ) )
	{
		scenarios.erase( it );
		ui.tabWidget->removeTab( idx );
	}
}

void SconeStudio::updateViewSettings()
{
	if ( model )
	{
		StudioModel::ViewFlags f;
		f.set( StudioModel::ShowForces, ui.actionShow_External_Forces->isChecked() );
		f.set( StudioModel::ShowMuscles, ui.actionShow_Muscles->isChecked() );
		f.set( StudioModel::ShowGeometry, ui.actionShow_Bone_Geometry->isChecked() );
		f.set( StudioModel::ShowAxes, ui.actionShow_Body_Axes->isChecked() );
		model->ApplyViewSettings( f );
	}

	ProgressDockWidget::AxisScaleType scale = ui.actionUse_Log_Scale->isChecked() ? ProgressDockWidget::Logarithmic : ProgressDockWidget::Linear;
	for ( auto& o : optimizations )
		o->SetAxisScaleType( scale );
}

void SconeStudio::fixViewCheckboxes()
{
	ui.actionOptimization_Results->blockSignals( true );
	ui.actionOptimization_Results->setChecked( ui.resultsDock->isVisible() );
	ui.actionOptimization_Results->blockSignals( false );

	ui.action_Messages->blockSignals( true );
	ui.action_Messages->setChecked( ui.messagesDock->isVisible() );
	ui.action_Messages->blockSignals( false );
}

void SconeStudio::updateTabTitles()
{
	for ( auto s : scenarios )
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
}

QCodeEditor* SconeStudio::getActiveScenario()
{
	for ( auto edw : scenarios )
	{
		if ( !edw->visibleRegion().isEmpty() )
			return edw;
	}
	return nullptr;
}

void SconeStudio::finalizeCapture()
{
	ui.osgViewer->stopCapture();

	QString program = make_qt( flut::get_application_folder() / SCONE_FFMPEG_EXECUTABLE );
	QStringList args;
	args << "-r" << QString::number( capture_frequency ) << "-i" << captureFilename + ".images/image_0_%d.png" << "-c:v" << "mpeg4" << "-q:v" << "3" << captureFilename;

	cout << "starting " << program.toStdString() << endl;
	auto v = args.toVector();
	for ( auto arg : v ) cout << arg.toStdString() << endl;

	captureProcess = new QProcess( this );
	captureProcess->start( program, args );

	flut_error_if( !captureProcess->waitForStarted( 5000 ), "Could not start process" );
	scone::log::info( "Generating video for ", captureFilename.toStdString() );

	if ( !captureProcess->waitForFinished( 30000 ) )
		scone::log::error( "Did not finish in time" );

	scone::log::info( "Video generated" );
	QDir( captureFilename + ".images" ).removeRecursively();

	delete captureProcess;
	captureProcess = nullptr;
	captureFilename.clear();
}

void SconeStudio::closeEvent( QCloseEvent *e )
{
	abortOptimizations();
	if ( !optimizations.empty() )
	{
		e->ignore();
		return;
	}

	flut::log::debug( "Saving GUI settings" );
	QSettings cfg( "SCONE", "SconeStudio" );
	cfg.setValue( "geometry", saveGeometry() );
	cfg.setValue( "windowState", saveState() );
	cfg.setValue( "recentFiles", recentFiles );

	QMainWindow::closeEvent( e );
}

void SconeStudio::performReflexAnalysis()
{
	if ( !model || model->IsEvaluating() )
		return ( void )QMessageBox::information( this, "Cannot perform analysis", "No model evaluated" );

	path par_file( currentParFile.toStdString() );

	ReflexAnalysisObjective reflex_objective( model->GetData(), "use_force=1;use_length=0;use_velocity=0" );
	reflex_objective.set_delays( load_prop( scone::GetFolder( SCONE_MODEL_FOLDER ) / "neural_delays.pn" ) );
	
	spot::file_reporter frep( par_file.replace_extension( "analysis" ) );
	spot::cma_optimizer cma( reflex_objective );
	cma.set_max_threads( 32 );
	cma.add_reporter( std::make_shared< spot::console_reporter >( 0, 2 ) );
	cma.add_reporter( std::make_shared< spot::file_reporter >( par_file.replace_extension( "analysis" ) ) );
	cma.run( 1000 );
	reflex_objective.save_report( par_file.replace_extension( "reflex_analysis" ), cma.best_point() );
}
