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

using namespace scone;
using namespace std;

SconeStudio::SconeStudio( QWidget *parent, Qt::WindowFlags flags ) :
QMainWindow( parent, flags ),
slomo_factor( 1 ),
com_delta( Vec3( 0, 1, 0 ) ),
close_all( false ),
capture_frequency( 30 ),
evaluation_time_step( 1.0 / 8 ),
captureProcess( nullptr )
{
	ui.setupUi( this );
	//setCentralWidget( nullptr );
	//setDockNestingEnabled( true );

	setCorner( Qt::TopLeftCorner, Qt::LeftDockWidgetArea );
	setCorner( Qt::BottomLeftCorner, Qt::LeftDockWidgetArea );
	//setCorner( Qt::TopRightCorner, Qt::RightDockWidgetArea );
	//setCorner( Qt::BottomRightCorner, Qt::BottomDockWidgetArea );

	addDockWidget( Qt::LeftDockWidgetArea, ui.resultsDock );
	//addDockWidget( Qt::RightDockWidgetArea, ui.viewerDock );
	addDockWidget( Qt::BottomDockWidgetArea, ui.messagesDock );
}

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
{
	// init file model and browser widget
	resultsFileModel = new QFileSystemModel( this );
	resultsFileModel->setNameFilters( QStringList( "*.par" ) );
	ui.resultsBrowser->setModel( resultsFileModel );
	ui.resultsBrowser->setRootIndex( resultsFileModel->setRootPath( QString( scone::GetFolder( SCONE_OUTPUT_FOLDER ).c_str() ) ) );
	for ( int i = 1; i <= 3; ++i ) ui.resultsBrowser->hideColumn( i );
	connect( ui.resultsBrowser->selectionModel(),
		SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&, const QModelIndex& ) ) );

	ui.osgViewer->setScene( manager.GetOsgRoot() );
	ui.tabWidget->removeTab( 1 );
	ui.tabWidget->tabBar()->tabButton( 0, QTabBar::RightSide )->resize( 0, 0 );

	// start timer for viewer
	connect( &qtimer, SIGNAL( timeout() ), this, SLOT( updateTimer() ) );
	connect( &backgroundUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateBackgroundTimer() ) );
	backgroundUpdateTimer.start( 1000 );

	return true;
}

void SconeStudio::add_log_entry( flut::log::level l, const std::string& msg )
{
#ifdef DEBUG
	cout << msg << endl;
#endif // DEBUG
	{
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
		cursor.insertText( QString( msg.c_str() ) + "\n" );
	}

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
		String filename = resultsFileModel->fileInfo( idx ).absoluteFilePath().toStdString();
		manager.CreateModel( filename );
		ui.horizontalScrollBar->setRange( 0, int( 1000 * manager.GetMaxTime() ) );
		ui.horizontalScrollBar->setDisabled( manager.IsEvaluating() );

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
	auto item = resultsFileModel->fileInfo( idx );
	string dirname = item.isDir() ? item.filePath().toStdString() : item.dir().path().toStdString();
	//log::trace( dirname );
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
	ui.speedButton1->setChecked( v == 1 );
	ui.speedButton4->setChecked( v == 4 );
	ui.speedButton16->setChecked( v == 16 );
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
	current_time = t;

	// update ui and visualization
	bool is_evaluating = manager.IsEvaluating();
	manager.Update( t );

	if ( current_time >= manager.GetMaxTime() )
	{
		current_time = manager.GetMaxTime();
		if ( qtimer.isActive() )
			stop();
	}

	auto d = com_delta( manager.GetModel().GetSimModel().GetComPos() );
	ui.osgViewer->moveCamera( osg::Vec3( d.x, 0, d.z ) );
	ui.osgViewer->repaint();

	ui.horizontalScrollBar->blockSignals( true );
	ui.doubleSpinBox->blockSignals( true );

	ui.horizontalScrollBar->setValue( 1000 * current_time );
	ui.doubleSpinBox->setValue( current_time );

	ui.horizontalScrollBar->blockSignals( false );
	ui.doubleSpinBox->blockSignals( false );

	// check if the evaluation has just finished
	if ( is_evaluating && !manager.IsEvaluating() )
	{
		ui.horizontalScrollBar->setEnabled( true );
		ui.horizontalScrollBar->setRange( 0, int( 1000 * manager.GetMaxTime() ) );
		setTime( 0 );
		start();
	}
}

void SconeStudio::fileOpen()
{
	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ), "SCONE Scenarios (*.xml)" );
	if ( !filename.isEmpty() )
	{
		EditorWidget* edw = new EditorWidget( this, filename );
		int idx = ui.tabWidget->addTab( edw, edw->getTitle() );
		ui.tabWidget->setCurrentIndex( idx );
		scenarios.push_back( edw );
		//edw->show();
		//edw->raise();
	}
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
		getActiveScenario()->saveAs();
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

	// divide into tabs
	if ( optimizations.size() >= 3 )
	{
		auto tab_count = ( optimizations.size() + 4 ) / 5;
		for ( size_t i = 0; i < optimizations.size(); ++ i )
		{
			addDockWidget( Qt::RightDockWidgetArea, optimizations[ i ] );
			if ( i % tab_count != 0 )
				//splitDockWidget( optimizations[ i / tab_count * tab_count ], optimizations[ i ] );
				splitDockWidget( optimizations[ i - 1 ], optimizations[ i ], Qt::Horizontal );
		}
	}
}

bool SconeStudio::checkAndSaveScenario( EditorWidget* s )
{
	if ( s == nullptr )
	{
		QMessageBox::information( this, "No Scenario Selected", "No Scenario open for editing" );
		return false;
	}

	if ( s->hasTextChanged() )
	{
		QString message = "Save changes to " + s->getTitle() + "?";
		auto ret = QMessageBox::warning( this, "Save Changes", message, QMessageBox::Save, QMessageBox::Discard );
		switch ( ret )
		{
		case QMessageBox::Save: s->save(); return true;
		case QMessageBox::Discard: return false;
		}
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
		int count = QInputDialog::getInt( this, "Run Multiple Optimizations", "Enter number of optimization instances: ", 3, 1, 100 );
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
		if ( w->isClosed() )
		{
			delete w;
			it = optimizations.erase( it );
		}
		else ++it;
	}

	// update all optimizations
	for ( auto& o : optimizations )
	{
		o->updateProgress();
		if ( o->state == ProgressDockWidget::ErrorState )
			o->close();
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

EditorWidget* SconeStudio::getActiveScenario()
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

	QString program = make_qt( flut::get_application_folder() + SCONE_FFMPEG_EXECUTABLE );
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
	if ( optimizations.empty() )
		e->accept();
	else e->ignore();
}
