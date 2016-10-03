#include "SconeStudio.h"
#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QApplication>
    #include <QtWidgets/QMessageBox.h>
#else
    #include <QtGui/QApplication>
#endif

#include "scone/cs/cs_tools.h"
#include "scone/sim/simbody/sim_simbody.h"

int main(int argc, char *argv[])
{
	// init SCONE. TODO: make this a single function
	scone::cs::RegisterFactoryTypes();
	scone::sim::RegisterSimbody();

	QApplication a(argc, argv);
	SconeStudio w;

	scone::log::SetLevel( scone::log::TraceLevel );
	flut::log::log_output_func f = std::bind( &SconeStudio::add_log_entry, &w, std::placeholders::_1, std::placeholders::_2 );
	flut::log::set_log_output_func( f );

	try
	{
#if QT_VERSION >= 0x050000
		// Qt5 is currently crashing and reporting "Cannot make QOpenGLContext current in a different thread" when the viewer is run multi-threaded, this is regression from Qt4
		osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::SingleThreaded;
#else
		osgViewer::ViewerBase::ThreadingModel threadingModel = osgViewer::ViewerBase::CullDrawThreadPerContext;
#endif

#if QT_VERSION >= 0x040800
		// Required for multithreaded QGLWidget on Linux/X11, see http://blog.qt.io/blog/2011/06/03/threaded-opengl-in-4-8/
		if (threadingModel != osgViewer::ViewerBase::SingleThreaded)
			QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif
	
		w.init(threadingModel);
		w.show();
		return a.exec();
	}
	catch ( std::exception& e )
	{
		QMessageBox::critical( 0, "Exception", e.what() );
	}
	catch ( ... )
	{
		QMessageBox::critical( 0, "Exception", "Unknown Exception" );
	}
}

#ifndef DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return main( __argc, __argv );
}
#endif
