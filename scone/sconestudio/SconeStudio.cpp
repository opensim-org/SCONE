#include "SconeStudio.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include <osgDB/ReadFile>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QFileSystemModel.h>
    #include <QtWidgets/QMessageBox.h>
#endif

#include "simvis/osg_tools.h"

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

	// init scenario browser
	scenarioFileModel = new QFileSystemModel( this );
	scenarioFileModel->setNameFilters( QStringList( "*.xml" ) );
	ui.scenarioBrowser->setModel( scenarioFileModel );
	ui.scenarioBrowser->setRootIndex( scenarioFileModel->setRootPath( QString( scone::GetFolder( SCONE_SCENARIO_FOLDER ).c_str() ) ) );
	for ( int i = 1; i <= 3; ++i ) ui.scenarioBrowser->hideColumn( i );

	// init osg viewer
	ui.osgViewer->setScene( manager.GetOsgRoot() );
	ui.stackedWidget->setCurrentIndex( 0 );

	// start timer for viewer
	connect( &qtimer, SIGNAL( timeout() ), this, SLOT( updateTimer() ) );

	return true;
}

SconeStudio::~SconeStudio() {}

void SconeStudio::activateBrowserItem( QModelIndex idx )
{
	try
	{
		if ( !manager.IsEvaluating() )
		{
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
