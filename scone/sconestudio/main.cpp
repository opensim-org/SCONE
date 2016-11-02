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
#include "scone/core/system_tools.h"
#include "qt_tools.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	// initialize scone
	scone::log::SetLevel( scone::log::TraceLevel );
	scone::cs::RegisterFactoryTypes();
	scone::sim::RegisterSimbody();

	QPixmap splash_pm( make_qt( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "resources/ui/scone_splash.png" ) );
	QSplashScreen splash( splash_pm );
	splash.show();

	//splash.showMessage( QString( "Initiating SCONE version " ) + scone::GetSconeVersion().to_str().c_str(), Qt::AlignLeft | Qt::AlignBaseline, Qt::black);
	a.processEvents();

	// init logging
	SconeStudio w;
	flut::log::log_output_func f = std::bind( &SconeStudio::add_log_entry, &w, std::placeholders::_1, std::placeholders::_2 );
	flut::log::set_log_output_func( f );
	scone::log::info( "SCONE version ", scone::GetSconeVersion() );

	// sleep a while so people can enjoy the splash screen :-)
	QThread::sleep( 1 );

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
		splash.close();
		
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
