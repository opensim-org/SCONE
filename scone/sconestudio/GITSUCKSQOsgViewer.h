#include <QWidget>
#include <QTimer>
#include <QApplication>
#include <QGridLayout>

#include <osgViewer/CompositeViewer>
#include <osgQt/GraphicsWindowQt>
#include <iostream>

#include "simvis/osg_camera_man.h"

class QOsgViewer : public QWidget, public osgViewer::CompositeViewer
{
public:
	QOsgViewer(QWidget* parent = 0, Qt::WindowFlags f = 0, osgViewer::ViewerBase::ThreadingModel threadingModel=osgViewer::CompositeViewer::SingleThreaded);
	QWidget* addViewWidget( osgQt::GraphicsWindowQt* gw );
	osgQt::GraphicsWindowQt* createGraphicsWindow( int x, int y, int w, int h, const std::string& name="", bool windowDecoration=false );

	virtual void paintEvent( QPaintEvent* event ) override { frame(); }

	void setScene( osg::Node* s );
	void moveCamera( const osg::Vec3d& delta_pos );

protected:
	QTimer _timer;
	osg::ref_ptr< vis::osg_camera_man > camera_man;
	osg::Node* scene;
};
