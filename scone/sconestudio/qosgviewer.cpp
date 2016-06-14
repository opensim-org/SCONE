#include "qosgviewer.h"

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>
#include "OsgCameraManipulator.h"

QOsgViewer::QOsgViewer( QWidget* parent /*= 0*/, Qt::WindowFlags f /*= 0*/, osgViewer::ViewerBase::ThreadingModel threadingModel/*=osgViewer::CompositeViewer::SingleThreaded*/ ) : QWidget( parent, f )
{
	setThreadingModel( threadingModel );

	// disable the default setting of viewer.done() by pressing Escape.
	setKeyEventSetsDone( 0 );

	QWidget* widget1 = addViewWidget( createGraphicsWindow( 0, 0, 100, 100, "", true ) );
	//QWidget* widget2 = addViewWidget( createGraphicsWindow(0,0,100,100, "", true ), scene );
	//auto* grid = new QVBoxLayout;
	auto* grid = new QGridLayout;
	grid->addWidget( widget1 );
	setLayout( grid );
	grid->setMargin( 1 );

	connect( &_timer, SIGNAL( timeout() ), this, SLOT( update() ) );
	_timer.start( 1 );
}

QWidget* QOsgViewer::addViewWidget( osgQt::GraphicsWindowQt* gw )
{
	osgViewer::View* view = new osgViewer::View;
	addView( view );

	osg::Camera* camera = view->getCamera();
	camera->setGraphicsContext( gw );

	const osg::GraphicsContext::Traits* traits = gw->getTraits();

	camera->setClearColor( osg::Vec4( 0.2, 0.2, 0.6, 1.0 ) );
	camera->setViewport( new osg::Viewport( 0, 0, traits->width, traits->height ) );
	camera->setProjectionMatrixAsPerspective( 30.0f, static_cast<double>( traits->width ) / static_cast<double>( traits->height ), 1.0f, 10000.0f );

	//view->setSceneData( scene );
	view->addEventHandler( new osgViewer::StatsHandler );

	// setup camera manipulator
	//osgGA::TrackballManipulator* cm = new osgGA::TrackballManipulator;
	scone::OsgCameraManipulator* cm = new scone::OsgCameraManipulator;
	cm->setVerticalAxisFixed( false );
	view->setCameraManipulator( cm );

	gw->setTouchEventsEnabled( true );

	return gw->getGLWidget();
}

osgQt::GraphicsWindowQt* QOsgViewer::createGraphicsWindow( int x, int y, int w, int h, const std::string& name/*=""*/, bool windowDecoration/*=false */ )
{
	osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
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

void QOsgViewer::setScene( osg::Node* s )
{
	for ( size_t i = 0; i < getNumViews(); ++i )
		getView( i )->setSceneData( s );
}
