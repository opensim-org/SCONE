#include "QOsgViewer.h"

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include "simvis/osg_camera_man.h"
#include "scone/core/Log.h"

using namespace scone;

class QOsgEventHandler : public osgGA::GUIEventHandler 
{ 
public: 
	virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa, osg::Object* o, osg::NodeVisitor* nv )
	{
		if ( ea.getEventType() == osgGA::GUIEventAdapter::RESIZE )
		{
			log::info( "Viewer resized to ", ea.getWindowWidth(), "x", ea.getWindowHeight() );
			return true;
		}
		else return false;
	}
}; 


QOsgViewer::QOsgViewer( QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/, osgViewer::ViewerBase::ThreadingModel threadingModel/*=osgViewer::CompositeViewer::SingleThreaded*/ ) :
QWidget( parent, f ),
capture_handler( nullptr ),
frame_count( 0 )
{
	setThreadingModel( threadingModel );

	// disable the default setting of viewer.done() by pressing Escape.
	setKeyEventSetsDone( 0 );

	// setup viewer grid
	QWidget* widget1 = addViewWidget( createGraphicsWindow( 0, 0, 100, 100, "", true ) );
	auto* grid = new QGridLayout;
	grid->addWidget( widget1 );
	setLayout( grid );
	grid->setMargin( 1 );

	// start timer
	// TODO: remove this -- only update after something has changed
	connect( &_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	_timer.start( 1000 / 120 );
}

QWidget* QOsgViewer::addViewWidget( osgQt::GraphicsWindowQt* gw )
{
	viewer = new osgViewer::View;
	addView( viewer );

	osg::Camera* camera = viewer->getCamera();
	camera->setGraphicsContext( gw );

	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	camera->setClearColor( osg::Vec4( 0.55, 0.55, 0.55, 1.0 ) );
	camera->setViewport( new osg::Viewport( 0, 0, traits->width, traits->height ) );
	camera->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>( traits->width ) / static_cast<double>( traits->height ), 1.0f, 10000.0f );

	// add event handlers
	viewer->addEventHandler( new osgViewer::StatsHandler );
	viewer->addEventHandler( new QOsgEventHandler );

	// disable lighting by default
	viewer->setLightingMode( osg::View::NO_LIGHT );

	// setup camera manipulator
	gw->setTouchEventsEnabled( true );

	camera_man = new vis::osg_camera_man();
	camera_man->setVerticalAxisFixed( false );
	viewer->setCameraManipulator( camera_man );

	return gw->getGLWidget();
}

osgQt::GraphicsWindowQt* QOsgViewer::createGraphicsWindow( int x, int y, int w, int h, const std::string& name/*=""*/, bool windowDecoration/*=false */ )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
	ds->setNumMultiSamples( 2 );

	osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
	traits->windowName = name;
	traits->windowDecoration = windowDecoration;
	traits->x = x;
	traits->y = y;
	traits->width = w;
	traits->height = h;
	traits->doubleBuffer = true;
	traits->alpha = ds->getMinimumNumAlphaBits();
	traits->stencil = ds->getMinimumNumStencilBits();
	traits->sampleBuffers = ds->getMultiSamples();
	traits->samples = ds->getNumMultiSamples();

	return new osgQt::GraphicsWindowQt( traits.get() );
}

void QOsgViewer::paintEvent( QPaintEvent* event )
{
	++frame_count;
	frame();
}

void QOsgViewer::setScene( osg::Node* s )
{
	for ( size_t i = 0; i < getNumViews(); ++i )
		getView( i )->setSceneData( s );
}

void QOsgViewer::moveCamera( const osg::Vec3d& delta_pos )
{
	camera_man->setCenter( camera_man->getCenter() + delta_pos );
}

void QOsgViewer::startCapture( const std::string& filename )
{
	// create capture handler
	log::info( "Started capturing video to ", filename );
	capture_handler = new osgViewer::ScreenCaptureHandler( 
		new osgViewer::ScreenCaptureHandler::WriteToFile( filename, "png", osgViewer::ScreenCaptureHandler::WriteToFile::SEQUENTIAL_NUMBER ), -1 );
	viewer->addEventHandler( capture_handler );
	capture_handler->startCapture();
}

void QOsgViewer::stopCapture()
{
	if ( capture_handler )
	{
		log::info( "Video capture stopped" );
		capture_handler->stopCapture();
		capture_handler = nullptr;
	}
}
