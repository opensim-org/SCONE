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

using namespace scone;
using namespace std;

SconeStudio::SconeStudio( QWidget *parent, Qt::WindowFlags flags ) :
QMainWindow( parent, flags ),
slomo_factor( 1 ),
com_delta( Vec3( 0, 1, 0 ) ),
close_all( false ),
capture_frequency( 60 )
{
	ui.setupUi( this );
	//setCentralWidget( nullptr );
	//setDockNestingEnabled( true );
}

bool SconeStudio::init( osgViewer::ViewerBase::ThreadingModel threadingModel )
{
	setDockNestingEnabled( true );

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
	showViewer();

	// init text editor
	ui.scenarioEdit->setTabStopWidth( 16 );
	ui.scenarioEdit->setWordWrapMode( QTextOption::NoWrap );
	xmlSyntaxHighlighter = new BasicXMLSyntaxHighlighter( ui.scenarioEdit );

	// start timer for viewer
	connect( &qtimer, SIGNAL( timeout() ), this, SLOT( updateTimer() ) );

	connect( &optimizationUpdateTimer, SIGNAL( timeout() ), this, SLOT( updateOptimizations() ) );
	optimizationUpdateTimer.start( 1000 );

	return true;
}

SconeStudio::~SconeStudio()
{
	delete xmlSyntaxHighlighter;
}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	try
	{
		if ( !manager.IsEvaluating() )
		{
			showViewer();
			String filename = resultsFileModel->fileInfo( idx ).absoluteFilePath().toStdString();
			manager.CreateModel( filename );
			ui.horizontalScrollBar->setRange( 0, int( 1000 * manager.GetMaxTime() ) );
			setTime( 0 );
			start();
		}
		else log::warning( "Cannot activate new model until current simulation is finished" );

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

void SconeStudio::resultsSelectionChanged( const QItemSelection& newitem, const QItemSelection& olditem )
{
	for ( auto& i : newitem.indexes() )
		cout << i.row() << endl;
}

void SconeStudio::updateScrollbar( int pos )
{
	setTime( double( pos ) / 1000 );
}

void SconeStudio::updateSpinBox( double value )
{
	setTime( value );
}

void SconeStudio::start()
{
	if ( current_time >= manager.GetMaxTime() )
		setTime( 0 );

	qtimer.start( 10 );
	timer.reset();
	timer_delta.reset();

	if ( !captureFilename.isEmpty() )
		ui.osgViewer->startCapture( captureFilename.toStdString() );
}

void SconeStudio::stop()
{
	if ( !captureFilename.isEmpty() )
	{
		ui.osgViewer->stopCapture();
		captureFilename.clear();
	}

	if ( qtimer.isActive() )
		qtimer.stop();
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
	TimeInSeconds dt = isRecording() ? ( 1 / capture_frequency ) : timer_delta( timer.seconds() );

	setTime( current_time + slomo_factor * dt );
	ui.horizontalScrollBar->blockSignals( false );
}

void SconeStudio::fileOpen()
{
	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ), "SCONE Scenarios (*.xml)" );
	if ( !filename.isEmpty() )
	{
		currentFilename = filename;
		fileChanged = false;
		QFile f( currentFilename );
		if ( f.open( QFile::ReadOnly | QFile::Text ) )
		{
			QTextStream str( &f );
			ui.scenarioEdit->setText( str.readAll() );
			showEditor();
		}
	}
}

void SconeStudio::fileSave()
{
	if ( !currentFilename.isEmpty() )
	{
		QFile file( currentFilename );
		if ( !file.open( QIODevice::WriteOnly ) )
		{
			QMessageBox::critical( this, "Error writing file", "Could not open file " + currentFilename );
			return;
		}
		else
		{
			QTextStream stream( &file );
			stream << ui.scenarioEdit->toPlainText();
			stream.flush();
			file.close();
		}
	}
}

void SconeStudio::fileSaveAs()
{
	QString filename = QFileDialog::getSaveFileName( this, "Save Scenario", QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ), "SCONE Scenarios (*.xml)" );
	if ( !filename.isEmpty() )
	{
		currentFilename = filename;
		fileSave();
	}
}

void SconeStudio::fileExit()
{
	log::info( "Exiting SCONE" );
	close();
}

void SconeStudio::optimizeScenario()
{
	if ( currentFilename.isEmpty() )
	{
		QMessageBox::information( this, "No Scenario Selected", "No Scenario open for editing" );
		return;
	}

	ProgressDockWidget* pdw = new ProgressDockWidget( this, currentFilename );
	optimizations.push_back( pdw );

	addDockWidget( optimizations.size() <= 2 ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea, pdw );

	// move to the right
	if ( optimizations.size() == 3 )
	{
		for ( auto& o : optimizations )
			addDockWidget( Qt::RightDockWidgetArea, o );
	}

	// divide into tabs
	if ( optimizations.size() >= 6 )
	{
		auto tab_count = ( optimizations.size() + 4 ) / 5;
		for ( size_t i = 0; i < optimizations.size(); ++ i )
		{
			if ( i % tab_count != 0 )
				//splitDockWidget( optimizations[ i / tab_count * tab_count ], optimizations[ i ] );
				splitDockWidget( optimizations[ i - 1 ], optimizations[ i ], Qt::Horizontal );
		}
	}

	updateOptimizations();
}

void SconeStudio::abortOptimizations()
{
	if ( optimizations.size() > 0 )
	{
		QString message = QString().sprintf( "Are you sure you want to terminate %d optimizations?", optimizations.size() );
		if ( QMessageBox::warning( this, "Terminate Optimizations", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Abort )
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
	}
}

void SconeStudio::createVideo()
{
	captureFilename = QFileDialog::getSaveFileName( this, "Video Filename", QString(), QString() );
}

void SconeStudio::setTime( TimeInSeconds t )
{
	if ( !manager.HasModel() )
		return;

	if ( manager.GetModel().IsEvaluating() )
	{
		// always set current time to most recent simulation time
		current_time = manager.GetMaxTime();
		ui.horizontalScrollBar->setRange( 0, int( 1000 * current_time ) );
	}
	else
	{
		// update current time and stop when done
		current_time = t;
		if ( current_time >= manager.GetMaxTime() )
		{
			current_time = manager.GetMaxTime();
			if ( qtimer.isActive() )
				stop();
		}
	}

	// update ui and visualization
	manager.Update( t );
	auto d = com_delta( manager.GetModel().GetSimModel().GetComPos() );
	ui.osgViewer->moveCamera( osg::Vec3( d.x, 0, d.z ) );
	ui.osgViewer->update();

	ui.horizontalScrollBar->blockSignals( true );
	ui.doubleSpinBox->blockSignals( true );

	ui.horizontalScrollBar->setValue( 1000 * current_time );
	ui.doubleSpinBox->setValue( current_time );

	ui.horizontalScrollBar->blockSignals( false );
	ui.doubleSpinBox->blockSignals( false );
}

void SconeStudio::closeEvent( QCloseEvent *e )
{
	abortOptimizations();
	if ( optimizations.empty() )
		e->accept();
	else e->ignore();
}
