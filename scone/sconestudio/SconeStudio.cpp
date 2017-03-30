#include "SconeStudio.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include <osgDB/ReadFile>

#include <QtWidgets/QFileSystemModel.h>
#include <QtWidgets/QMessageBox.h>
#include <QtWidgets/QFileDialog>
#include <QTextStream>

#include "simvis/osg_tools.h"
#include "scone/opt/opt_tools.h"
#include "flut/system_tools.hpp"
#include "qt_tools.h"
#include "qevent.h"
#include "qcustomplot.h"
#include "studio_config.h"
#include "ui_SconeSettings.h"
#include "scone/core/Profiler.h"

using namespace scone;
using namespace std;

SconeStudio::SconeStudio( QWidget *parent, Qt::WindowFlags flags ) :
QCompositeMainWindow( parent, flags ),
slomo_factor( 1 ),
com_delta( Vec3( 0, 1, 0 ) ),
close_all( false ),
capture_frequency( 30 ),
evaluation_time_step( 1.0 / 8 ),
captureProcess( nullptr )
{
	flut::log::debug( "Constructing UI elements" );
	ui.setupUi( this );
	ui.stackedWidget->setCurrentIndex( 0 );

	analysisView = new QDataAnalysisView( &storageModel, this );
	analysisView->setObjectName( "Analysis" );

	// create window menu
	auto* actionMenu = menuBar()->addMenu( "&Action" );
	addMenuAction( actionMenu, "Toggle &Play", ui.playControl, &QPlayControl::play, Qt::Key_F5 );
	addMenuAction( actionMenu, "&Stop / Reset", ui.playControl, &QPlayControl::stop, Qt::Key_F8, true );
	addMenuAction( actionMenu, "&Test Current Scenario", this, &SconeStudio::runScenario, QKeySequence( "Ctrl+T" ) );

	createWindowMenu();
	createHelpMenu();

	setDockNestingEnabled( true );
	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::BottomDockWidgetArea );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	registerDockWidget( ui.resultsDock, "Optimization &Results" );
	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
	registerDockWidget( ui.messagesDock, "&Messages" );
	auto* adw = createDockWidget( "&Analysis", analysisView, Qt::BottomDockWidgetArea );
	tabifyDockWidget( ui.messagesDock, adw );
}

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
{
	// init file model and browser widget
	auto results_folder = make_qt( scone::GetFolder( SCONE_RESULTS_FOLDER ) );
	QDir().mkdir( results_folder );
	ui.resultsBrowser->setRoot( results_folder, "*.par" );

	connect( ui.resultsBrowser->selectionModel(),
		SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&, const QModelIndex& ) ) );

	ui.osgViewer->setScene( manager.GetOsgRoot() );
	ui.tabWidget->tabBar()->tabButton( 0, QTabBar::RightSide )->resize( 0, 0 );

	ui.playControl->setRange( 0, 100 );
	connect( ui.playControl, &QPlayControl::playTriggered, this, &SconeStudio::start );
	connect( ui.playControl, &QPlayControl::stopTriggered, this, &SconeStudio::stop );
	connect( ui.playControl, &QPlayControl::timeChanged, this, &SconeStudio::setTime );
	connect( ui.playControl, &QPlayControl::sliderReleased, this, &SconeStudio::refreshAnalysis );
	connect( ui.playControl, &QPlayControl::nextTriggered, this, &SconeStudio::refreshAnalysis );
	connect( ui.playControl, &QPlayControl::previousTriggered, this, &SconeStudio::refreshAnalysis );

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
{
}

void SconeStudio::runSimulation( const QString& filename )
{
	try
	{
		showViewer();
		ui.playControl->stop();
		ui.playControl->reset();
		manager.CreateModel( filename.toStdString() );
		ui.playControl->setRange( 0, manager.GetMaxTime() );
		storageModel.setStorage( &manager.GetModel().GetData() );
		analysisView->reset();

		if ( manager.IsEvaluating() )
		{
			ui.playControl->setDisabled( true );
			evaluate();
		}

		ui.playControl->setRange( 0, manager.GetMaxTime() );
		ui.playControl->setDisabled( false );
		ui.playControl->reset();
		ui.playControl->play();
	}
	catch ( std::exception& e )
	{
		QMessageBox::critical( this, "Exception", e.what() );
	}

}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	currentParFile = ui.resultsBrowser->fileSystemModel()->fileInfo( idx ).absoluteFilePath();
	runSimulation( currentParFile );
}

void SconeStudio::selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold )
{
	auto item = ui.resultsBrowser->fileSystemModel()->fileInfo( idx );
	string dirname = item.isDir() ? item.filePath().toStdString() : item.dir().path().toStdString();
}

void SconeStudio::start()
{
	ui.osgViewer->stopTimer();

	if ( !captureFilename.isEmpty() )
	{
		QDir().mkdir( captureFilename + ".images" );
		ui.osgViewer->startCapture( captureFilename.toStdString() + ".images/image" );
	}
}

void SconeStudio::stop()
{
	if ( !captureFilename.isEmpty() )
		finalizeCapture();

	ui.osgViewer->startTimer();
	refreshAnalysis();
}

void SconeStudio::refreshAnalysis()
{
	analysisView->refresh( current_time );
}

void SconeStudio::evaluate()
{
	ui.abortButton->setChecked( false );
	ui.progressBar->setValue( 0 );
	ui.stackedWidget->setCurrentIndex( 1 );
	ui.progressBar->setTextVisible( true );

	SCONE_PROFILE_RESET;
	const double step_size = 0.2;
	for ( double t = step_size; t < manager.GetMaxTime(); t += step_size )
	{
		ui.progressBar->setValue( int( t / manager.GetMaxTime() * 100 ) );
		QApplication::processEvents();
		if ( ui.abortButton->isChecked() )
		{
			manager.GetModel().FinalizeEvaluation( false );
			ui.stackedWidget->setCurrentIndex( 0 );
			return;
		}
		setTime( t );
	}
	ui.progressBar->setValue( 100 );
	manager.Update( manager.GetMaxTime() );
	ui.stackedWidget->setCurrentIndex( 0 );

	log::info( SCONE_PROFILE_REPORT );
}

void SconeStudio::setTime( TimeInSeconds t )
{
	SCONE_PROFILE_FUNCTION;

	if ( !manager.HasModel() )
		return;

	// update current time and stop when done
	current_time = t;

	// update ui and visualization
	//bool is_evaluating = manager.IsEvaluating();
	manager.Update( t );

	auto d = com_delta( manager.GetModel().GetSimModel().GetComPos() );
	ui.osgViewer->moveCamera( osg::Vec3( d.x, 0, d.z ) );
	ui.osgViewer->repaint();

	// update graph (if visible)
	if ( analysisView->isVisible() )
		analysisView->refresh( current_time, false );
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
	if ( getActiveScenario() )
	{
		//log::trace( "Active scenario: ", getActiveScenario()->fileName.toStdString() );
		getActiveScenario()->save();
	}
}

void SconeStudio::fileSaveAs()
{
	if ( getActiveScenario() )
		getActiveScenario()->saveAsDialog( make_qt( scone::GetFolder( SCONE_SCENARIO_FOLDER ) ), "SCONE Scenarios (*.xml)" );
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
		auto tab_count = ( optimizations.size() + 3 ) / 4;
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

	return true;
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

void SconeStudio::createVideo()
{
	captureFilename = QFileDialog::getSaveFileName( this, "Video Filename", QString(), "avi files (*.avi)" );
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
	if ( manager.HasModel() )
	{
		manager.GetModel().SetViewSetting( scone::StudioModel::ShowForces, ui.actionShow_External_Forces->isChecked() );
		manager.GetModel().SetViewSetting( scone::StudioModel::ShowMuscles, ui.actionShow_Muscles->isChecked() );
		manager.GetModel().SetViewSetting( scone::StudioModel::ShowGeometry, ui.actionShow_Bone_Geometry->isChecked() );
	}
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
