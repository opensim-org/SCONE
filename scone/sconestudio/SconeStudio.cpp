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

#ifdef _MSC_VER
	const char* scone_program_name = "sconeopt.exe";
#else
	const char* scone_program_name = "sconeopt";
#endif


using namespace scone;
using namespace std;

SconeStudio::SconeStudio(QWidget *parent, Qt::WindowFlags flags) :
QMainWindow(parent, flags),
slomo_factor( 1 ),
com_delta( Vec3( 0, 1, 0 ) )
{
	ui.setupUi(this);
	//setCentralWidget( nullptr );
	//setDockNestingEnabled( true );
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
		SIGNAL( currentChanged( const QModelIndex&,const QModelIndex& ) ),
		this, SLOT( selectBrowserItem( const QModelIndex&,const QModelIndex& ) ) );

	// init scenario browser
	//scenarioFileModel = new QFileSystemModel( this );
	//scenarioFileModel->setNameFilters( QStringList( "*.xml" ) );
	//ui.scenarioBrowser->setModel( scenarioFileModel );
	//ui.scenarioBrowser->setRootIndex( scenarioFileModel->setRootPath( QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ) ) );
	//for ( int i = 1; i <= 3; ++i ) ui.scenarioBrowser->hideColumn( i );
	//ui.scenarioDock->hide();

	// init osg viewer
	ui.osgViewer->setScene( manager.GetOsgRoot() );
	showViewer();

	// init text editor
	ui.scenarioEdit->setTabStopWidth( 16 );
	ui.scenarioEdit->setWordWrapMode( QTextOption::NoWrap );
	xmlSyntaxHighlighter = new BasicXMLSyntaxHighlighter( ui.scenarioEdit );
	
	// init optimizations list
	ui.optTree->setHeaderLabels( QStringList{ "Name", "Status" } );
	ui.optTree->addTopLevelItem( new QTreeWidgetItem( QStringList{ "Jazeker", "Meneer" } ) );

	// start timer for viewer
	connect( &qtimer, SIGNAL( timeout() ), this, SLOT( updateTimer() ) );

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
		else log::warning( "Cannot activate new model until current simulation is finished");

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
	log::trace( dirname );
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
}

void SconeStudio::stop()
{
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
	setTime( current_time + slomo_factor * timer_delta( timer.seconds() ) );
	ui.horizontalScrollBar->blockSignals( false );
}

void SconeStudio::fileOpen()
{
	QString filename = QFileDialog::getOpenFileName( this, "Open Scenario", QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ), "SCONE Scenarios (*.xml)" );
	if ( !filename.isEmpty() )
	{
		currentFileName = filename;
		fileChanged = false;
		QFile f( currentFileName );
		if ( f.open( QFile::ReadOnly | QFile::Text) )
		{
			QTextStream str( &f );
			ui.scenarioEdit->setText( str.readAll() );		
			showEditor();		
		}
	}
}

void SconeStudio::fileSave()
{

}

void SconeStudio::fileSaveAs()
{

}

void SconeStudio::fileExit()
{
	log::info( "Exiting SCONE" );
	close();
}

void SconeStudio::optimizeScenario()
{
	if ( currentFileName.isEmpty() )
	{
		QMessageBox::information( this, "No Scenario Selected", "No Scenario open for editing" );
		return;
	}

	QString program = make_qt( flut::get_application_folder() + scone_program_name );
	QStringList args;
	args << currentFileName;
	QProcess* p = new QProcess( this );
	p->setReadChannel( QProcess::StandardOutput );
	p->start( program, args );

	flut_error_if( !p->waitForStarted( 3000 ), "Could not start process" );

	for ( int i = 0; i < 20; ++i )
	{
		if ( p->waitForReadyRead( 1000 ) )
		{
			string s = QString::fromLocal8Bit( p->readLine() ).toStdString();
			auto kvp = flut::to_key_value( s );
			if ( kvp.first == "folder" )
			{
				p->setObjectName( make_qt( kvp.second ) );
				break;
			}
		}
	}

	log::info( "Started optimization ", p->objectName().toStdString() );
	processes.push_back( p );
}

void SconeStudio::abortOptimizations()
{
	if ( processes.size() > 0 )
	{
		QString message = QString().sprintf( "Are you sure you want to terminate %d optimizations?", processes.size() );
		if ( QMessageBox::warning( this, "Terminate Optimizations", message, QMessageBox::Abort, QMessageBox::Cancel ) == QMessageBox::Abort )
		{
			for ( auto* p : processes )
			{
				log::info( "Closing process ", p->objectName().toStdString() );
				p->close();
				delete p;
			}
			processes.clear();
		}
	}
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
	if ( processes.empty() )
		e->accept();
	else e->ignore();
}
