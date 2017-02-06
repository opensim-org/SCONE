#include "SconeStudio.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include <osgDB/ReadFile>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#	include <QtWidgets/QFileSystemModel.h>
#	include <QtWidgets/QMessageBox.h>
#	include <QtWidgets/QFileDialog>
#endif
#include <QTextStream>

#include "simvis/osg_tools.h"
#include "scone/opt/opt_tools.h"
#include "flut/system_tools.hpp"
#include "qt_tools.h"
#include "qevent.h"
#include "qcustomplot.h"
#include "studio_config.h"
#include "ui_SconeSettings.h"

using namespace scone;
using namespace std;

SconeStudio::SconeStudio( QWidget *parent, Qt::WindowFlags flags ) :
QMainWindow( parent, flags ),
slomo_factor( 1 ),
com_delta( Vec3( 0, 1, 0 ) ),
close_all( false ),
capture_frequency( 30 ),
evaluation_time_step( 1.0 / 8 ),
captureProcess( nullptr ),
enableLogging( false ),
flut::log::sink( flut::log::trace_level )
{
	flut::log::debug( "Constructing UI elements" );
	ui.setupUi( this );

	//setDockNestingEnabled( true );

	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	setCorner( Qt::BottomRightCorner, Qt::BottomDockWidgetArea );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
}

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
{
	flut::log::debug( "Initializing results window" );

	// init file model and browser widget
	auto results_folder = make_qt( scone::GetFolder( SCONE_RESULTS_FOLDER ) );
	QDir().mkdir( results_folder );
	ui.resultsBrowser->setFolder( results_folder, "*.par" );

	connect( ui.resultsBrowser->selectionModel(),
		SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&, const QModelIndex& ) ) );

	flut::log::debug( "Initializing viewer window" );
	ui.osgViewer->setScene( manager.GetOsgRoot() );
	ui.tabWidget->tabBar()->tabButton( 0, QTabBar::RightSide )->resize( 0, 0 );

	flut::log::debug( "Initializing play control widget" );
	ui.playControl->setRange( 0, 10000 );
	connect( ui.playControl, SIGNAL( play() ), this, SLOT( start() ) );
	connect( ui.playControl, SIGNAL( stop() ), this, SLOT( stop() ) );
	connect( ui.playControl, SIGNAL( previous() ), this, SLOT( previous() ) );
	connect( ui.playControl, SIGNAL( next() ), this, SLOT( next() ) );
	connect( ui.playControl, SIGNAL( sliderChanged(int) ), this, SLOT( updateScrollbar(int) ) );
	connect( ui.playControl, SIGNAL( slowMotionChanged(int) ), this, SLOT( slomo(int) ) );

	// start timer for viewer
	flut::log::debug( "Creating background timers" );
	connect( &qtimer, SIGNAL( timeout() ), this, SLOT( updateTimer() ) );
	connect( &backgroundUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateBackgroundTimer() ) );
	backgroundUpdateTimer.start( 1000 );

	// only do this after the ui has been initialized
	flut::log::debug( "Enabling messages window" );
	enableLogging = true; 

	flut::log::debug( "Loading GUI settings" );
	QSettings cfg( "SCONE", "SconeStudio" );
	restoreGeometry( cfg.value( "geometry" ).toByteArray() );
	restoreState( cfg.value( "windowState" ).toByteArray() );

	return true;
}

void SconeStudio::send_log_message( flut::log::level l, const string& msg )
{
	if ( !enableLogging )
		return; // we are not ready to start logging

	// remove newlines
	string trimmed_msg = flut::trim_right_str( msg );

	ui.outputText->moveCursor( QTextCursor::End );
	QTextCursor cursor( ui.outputText->textCursor() );
	QTextCharFormat format;
	format.setFontWeight( QFont::Normal );
	format.setForeground( QBrush( Qt::black ) );

	switch ( l )
	{
	case flut::log::trace_level:
	case flut::log::debug_level:
		format.setForeground( QBrush( Qt::gray ) );
		break;
	case flut::log::info_level:
		format.setForeground( QBrush( Qt::darkBlue ) );
		break;
	case flut::log::warning_level:
		format.setFontWeight( QFont::DemiBold );
		format.setForeground( QBrush( Qt::darkYellow ) );
		break;
	case flut::log::error_level:
	case flut::log::critical_level:
		format.setFontWeight( QFont::DemiBold );
		format.setForeground( QBrush( Qt::darkRed ) );
		break;
	default:
		break;
	}

	cursor.setCharFormat( format );
	cursor.insertText( make_qt( trimmed_msg ) + "\n" );

	ui.outputText->verticalScrollBar()->setValue( ui.outputText->verticalScrollBar()->maximum() );
}

SconeStudio::~SconeStudio()
{
}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	try
	{
		showViewer();
		String filename = ui.resultsBrowser->fileSystemModel()->fileInfo( idx ).absoluteFilePath().toStdString();
		manager.CreateModel( filename );
		ui.playControl->setRange( 0, int( 1000 * manager.GetMaxTime() ) );
		ui.playControl->setDisabled( manager.IsEvaluating() );

		setTime( 0 );
		start();
	}
	catch ( std::exception& e )
	{
		QMessageBox::critical( this, "Exception", e.what() );
	}
}

void SconeStudio::selectBrowserItem( const QModelIndex& idx, const QModelIndex& idxold )
{
	auto item = ui.resultsBrowser->fileSystemModel()->fileInfo( idx );
	string dirname = item.isDir() ? item.filePath().toStdString() : item.dir().path().toStdString();
}

void SconeStudio::start()
{
	if ( current_time >= manager.GetMaxTime() )
		setTime( 0 );

	ui.osgViewer->stopTimer();
	qtimer.start( 10 );

	timer.reset();
	timer_delta.reset();

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

	if ( qtimer.isActive() )
	{
		qtimer.stop();
		ui.osgViewer->startTimer();
	}
	else setTime( 0 );
}

void SconeStudio::slomo( int v )
{
	slomo_factor = 1.0 / v;
}

void SconeStudio::updateTimer()
{
	TimeInSeconds dt;
	if ( isRecording() )
		dt = slomo_factor / capture_frequency;
	else if ( isEvalutating() )
		dt = evaluation_time_step;
	else
		dt = slomo_factor * timer_delta( timer.seconds() );

	setTime( current_time + dt );
}

void SconeStudio::setTime( TimeInSeconds t )
{
	if ( !manager.HasModel() )
		return;

	// update current time and stop when done
	current_time = std::max( 0.0, t );

	// update ui and visualization
	bool is_evaluating = manager.IsEvaluating();
	manager.Update( t );

	if ( current_time > manager.GetMaxTime() )
	{
		if ( !ui.playControl->getLoop() )
		{
			current_time = manager.GetMaxTime();
			if ( qtimer.isActive() )
				stop();
		}
		else current_time = 0.0; // just reset
	}

	auto d = com_delta( manager.GetModel().GetSimModel().GetComPos() );
	ui.osgViewer->moveCamera( osg::Vec3( d.x, 0, d.z ) );
	ui.osgViewer->repaint();

	ui.playControl->setTime( current_time );

	// check if the evaluation has just finished
	if ( is_evaluating && !manager.IsEvaluating() )
	{
		ui.playControl->setEnabled( true );
		ui.playControl->setRange( 0, int( 1000 * manager.GetMaxTime() ) );
		setTime( 0 );
		start();
	}
}

void SconeStudio::fileOpen()
{
	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", make_qt( scone::GetFolder( SCONE_SCENARIO_FOLDER ) ), "SCONE Scenarios (*.xml)" );
	if ( !filename.isEmpty() )
	{
		QCodeEditor* edw = new QCodeEditor( this );
		edw->open( filename );
		int idx = ui.tabWidget->addTab( edw, edw->getTitle() );
		ui.tabWidget->setCurrentIndex( idx );
		scenarios.push_back( edw );
		//edw->show();
		//edw->raise();
	}
}

void SconeStudio::fileOpenRecent()
{
	SCONE_THROW_NOT_IMPLEMENTED;
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

void SconeStudio::addRecentFile( QString& filename )
{
	SCONE_THROW_NOT_IMPLEMENTED;
}

void SconeStudio::updateRecentFilesMenu()
{
	SCONE_THROW_NOT_IMPLEMENTED;
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
	scone::StudioModel::ViewFlags f;
	manager.GetModel().SetViewSetting( scone::StudioModel::ShowForces, ui.actionShow_External_Forces->isChecked() );
	manager.GetModel().SetViewSetting( scone::StudioModel::ShowMuscles, ui.actionShow_Muscles->isChecked() );
	manager.GetModel().SetViewSetting( scone::StudioModel::ShowGeometry, ui.actionShow_Bone_Geometry->isChecked() );
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

	QMainWindow::closeEvent( e );
}
