#include "SconeStudio.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include <osgDB/ReadFile>

#include <QtWidgets/QFileSystemModel.h>
#include <QtWidgets/QMessageBox.h>
#include <QtWidgets/QFileDialog>
#include <QTextStream>

#include "simvis/osg_tools.h"
#include "simvis/plane.h"
#include "scone/optimization/opt_tools.h"
#include "xo/system/system_tools.h"
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
#include "xo/filesystem/filesystem.h"
#include "simvis/color.h"
#include "scone/core/Settings.h"
#include "StudioSettings.h"
#include "QTabWidget"
#include "xo/utility/types.h"

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
scene_( true )
{
	xo::log::debug( "Constructing UI elements" );
	//setStyleSheet( "QFrame { border: 1px solid red }" );

	ui.setupUi( this );

	ui.stackedWidget->setCurrentIndex( 0 );
	ui.playControl->setDigits( 6, 3 );

	analysisView = new QDataAnalysisView( &storageModel, this );
	analysisView->setObjectName( "Analysis" );
	analysisView->setMinSeriesInterval( 0 );

	auto toolsMenu = menuBar()->addMenu( "&Tools" );
	addMenuAction( toolsMenu, "Generate &Video...", this, &SconeStudio::createVideo );
	addMenuAction( toolsMenu, "Save &Image...", this, &SconeStudio::captureImage, QKeySequence( "Ctrl+I" ), true );
	addMenuAction( toolsMenu, "&Preferences...", this, &SconeStudio::showSettingsDialog );

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
	//addMenuAction( actionMenu, "&Evaluate Current Scenario", this, &SconeStudio::runScenario, QKeySequence( "Ctrl+T" ) );

	createWindowMenu();
	createHelpMenu();

	setDockNestingEnabled( true );
	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::RightDockWidgetArea );
	//setTabPosition( Qt::AllDockWidgetAreas, QTabWidget::North );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	registerDockWidget( ui.resultsDock, "Optimization &Results" );
	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
	registerDockWidget( ui.messagesDock, "&Messages" );
	auto* adw = createDockWidget( "&Analysis", analysisView, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, adw );

	// init scene
	scene_.add_light( vis::vec3f( -20, 80, 40 ), vis::make_white( 1 ) );
	//ground_plane = scene.add< vis::plane >( 64, 64, 1, scone::GetStudioSetting< vis::color >( "viewer.tile1" ), scone::GetStudioSetting< vis::color >( "viewer.tile2" ) );
	ground_plane = scene_.add< vis::plane >( xo::vec3f( 64, 0, 0 ), xo::vec3f( 0, 0, -64 ), GetFolder( SCONE_UI_RESOURCE_FOLDER ) / "stile160.png", 64, 64 );
	ui.osgViewer->setClearColor( make_osg( scone::GetStudioSetting< vis::color >( "viewer.background" ) ) );
}

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
{
	// init file model and browser widget
	auto results_folder = scone::GetFolder( SCONE_RESULTS_FOLDER );
	xo::create_directories( results_folder );

	resultsModel = new ResultsFileSystemModel( nullptr );
	ui.resultsBrowser->setModel( resultsModel );
	ui.resultsBrowser->setNumColumns( 1 );
	ui.resultsBrowser->setRoot( make_qt( results_folder ), "*.par" );
	ui.resultsBrowser->header()->setFrameStyle( QFrame::NoFrame | QFrame::Plain );


	connect( ui.resultsBrowser->selectionModel(),
		SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&, const QModelIndex& ) ) );

	ui.osgViewer->setScene( &scene_ );
	ui.osgViewer->setHud( GetFolder( SCONE_UI_RESOURCE_FOLDER ) / "scone_hud.png" );
	//ui.tabWidget->tabBar()->tabButton( 0, QTabBar::RightSide )->resize( 0, 0 );

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
	backgroundUpdateTimer.start( 500 );

	// only do this after the ui has been initialized
	xo::log::add_sink( ui.outputText );
	ui.outputText->set_log_level( xo::log::trace_level );

	xo::log::debug( "Loading GUI settings" );
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
	SCONE_ASSERT( model );

	QProgressDialog dlg( ( "Evaluating " + model->GetFileName().string() ).c_str(), "Abort", 0, 1000, this );
	dlg.setWindowModality( Qt::WindowModal );

	const double step_size = 0.05;
	int vis_step = 0;
	xo::timer real_time;
	for ( double t = step_size; t < model->GetMaxTime(); t += step_size )
	{
		dlg.setValue( int( 1000 * t / model->GetMaxTime() ) );
		if ( dlg.wasCanceled() )
		{
			model->FinalizeEvaluation( false );
			break;
		}
		setTime( t, vis_step++ % 5 == 0 );
	}

	// make sure evaluation is finished
	if ( model->IsEvaluating() )
		model->EvaluateTo( model->GetMaxTime() );

	// report duration
	auto real_dur = real_time.seconds();
	auto sim_time = model->GetTime();
	log::info( "Evaluation took ", real_dur, "s for ", sim_time, "s (", sim_time / real_dur, "x real-time)" );

	dlg.setValue( 1000 );
	model->UpdateVis( model->GetTime() );
}

void SconeStudio::createVideo()
{
	if ( !model )
		return error( "No Scenario", "There is no scenario open" );

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
			if ( analysisView->isVisible() ) // TODO: not update so much when not playing (it's slow)
				analysisView->refresh( current_time, !ui.playControl->isPlaying() );
		}
	}
}

void SconeStudio::fileOpen()
{
	QString default_path = make_qt( GetFolder( SCONE_SCENARIO_FOLDER ) );
	if ( auto* s = getActiveScenario() )
		default_path = make_qt( path( s->fileName.toStdString() ).parent_path() );

	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", default_path, "SCONE Scenarios (*.scone *.xml *.zml)" );
	if ( !filename.isEmpty() )
		fileOpen( filename );
}

void SconeStudio::fileOpen( const QString& filename )
{
	try
	{
		QCodeEditor* edw = new QCodeEditor( this );
		edw->open( filename );
		int idx = ui.tabWidget->addTab( edw, edw->getTitle() );
		ui.tabWidget->setCurrentIndex( idx );
		connect( edw, &QCodeEditor::textChanged, this, &SconeStudio::updateTabTitles );
		scenarios.push_back( edw );
		addRecentFile( filename );
	}
	catch ( std::exception& e )
	{
		error( "Error opening file", e.what() );
	}
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
		s->save();
		ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
	}
}

void SconeStudio::fileSaveAs()
{
	if ( auto* s = getActiveScenario() )
	{
		QString fn = QFileDialog::getSaveFileName( this, "Save File As", s->fileName, "SCONE file (*.scone);;XML file (*.xml)" );
		if ( !fn.isEmpty() )
		{
			s->saveAs( fn );
			ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
			addRecentFile( s->fileName );
		}
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

	if ( optimizations.size() < 3 )
	{
		addDockWidget( optimizations.size() < 3 ? Qt::BottomDockWidgetArea : Qt::LeftDockWidgetArea, pdw );
	}
	// organize into columns
	if ( optimizations.size() >= 3 )
	{ 
		auto columns = std::max<int>( 1, ( optimizations.size() + 5 ) / 6 );
		auto rows = ( optimizations.size() + columns - 1 ) / columns;
		log::info( "Reorganizing windows, columns=", columns, " rows=", rows );

		// first column
		splitDockWidget( ui.viewerDock, optimizations[ 0 ], Qt::Horizontal );
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
		storageModel.setStorage( nullptr );
		model = StudioModelUP( new StudioModel( scene_, path( par_file ), force_evaluation ) );
	}
	catch ( std::exception& e )
	{
		error( "Could not create model", e.what() );
		return false;
	}
	return true;
}

bool SconeStudio::requestSaveChanges( QCodeEditor* s )
{
	if ( s )
	{
		if ( s->hasTextChanged() )
		{
			QString message = "Save changes to " + s->getTitle() + "?";
			if ( QMessageBox::warning( this, "Save Changes", message, QMessageBox::Save, QMessageBox::Discard ) == QMessageBox::Save )
				s->save();
			ui.tabWidget->setTabText( getTabIndex( s ), s->getTitle() );
			return true;
		}
		else return false;
	}
	else return information( "No Scenario Selected", "No Scenario open for editing" ), false;
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
	if ( auto* s = getActiveScenario( true ) )
	{
		requestSaveChanges( s );
		ProgressDockWidget* pdw = new ProgressDockWidget( this, getActiveScenario()->fileName );
		addProgressDock( pdw );
		updateOptimizations();
	}
}

void SconeStudio::runScenario()
{
	ui.playControl->stop();

	if ( auto* s = getActiveScenario( false ) )
	{
		requestSaveChanges( s );
		runSimulation( s->fileName );
		if ( model )
			ui.playControl->play();
	}
}

void SconeStudio::optimizeScenarioMultiple()
{
	if ( auto* s = getActiveScenario( true ) )
	{
		requestSaveChanges( s );

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
			QString msg = o->message;
			o->close();
			QMessageBox::critical( this, title, msg );
			return; // must return here because close invalidates the iterator
		}
	}
}

void SconeStudio::tabCloseRequested( int idx )
{
	auto it = xo::find( scenarios, (QCodeEditor*)ui.tabWidget->widget( idx ) );
	SCONE_THROW_IF( it == scenarios.end(), "Could not find scenarion for tab " + to_str( idx ) );

	requestSaveChanges( *it );
	scenarios.erase( it );
	ui.tabWidget->removeTab( idx );
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
		f.set( StudioModel::ShowContactGeom, ui.actionShow_Contact_Geometry->isChecked() );
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

QCodeEditor* SconeStudio::getActiveScenario( bool show_error )
{
	for ( auto edw : scenarios )
	{
		if ( !edw->visibleRegion().isEmpty() )
			return edw;
	}

	if ( show_error )
		QMessageBox::information( this, "No Scenario Selected", "No Scenario open for editing" );

	return nullptr;
}

void SconeStudio::finalizeCapture()
{
	ui.osgViewer->stopCapture();

	QString program = make_qt( xo::get_application_folder() / SCONE_FFMPEG_EXECUTABLE );
	QStringList args;
	args << "-r" << QString::number( capture_frequency ) << "-i" << captureFilename + ".images/image_0_%d.png" << "-c:v" << "mpeg4" << "-q:v" << "3" << captureFilename;

	cout << "starting " << program.toStdString() << endl;
	auto v = args.toVector();
	for ( auto arg : v ) cout << arg.toStdString() << endl;

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

void SconeStudio::closeEvent( QCloseEvent *e )
{
	abortOptimizations();
	if ( !optimizations.empty() )
	{
		e->ignore();
		return;
	}

	xo::log::debug( "Saving GUI settings" );
	QSettings cfg( "SCONE", "SconeStudio" );
	cfg.setValue( "geometry", saveGeometry() );
	cfg.setValue( "windowState", saveState() );
	cfg.setValue( "recentFiles", recentFiles );

	QMainWindow::closeEvent( e );
}
