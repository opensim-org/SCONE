#include "SconeStudio.h"
#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QApplication>
    #include <QtWidgets/QMessageBox.h>
#else
    #include <QtGui/QApplication>
#endif

#include "scone/controllers/cs_tools.h"
#include "scone/model/simbody/sim_simbody.h"
#include "scone/core/system_tools.h"
#include "qt_tools.h"
#include "xo/system/log_sink.h"
#include "xo/system/system_tools.h"
#include "scone/core/string_tools.h"

int main( int argc, char *argv[] )
{
	QApplication a( argc, argv );
	QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );

	QApplication::style()->setProperty( "margin", 50 );

	// init logging
	QDir().mkdir( make_qt( scone::GetSettingsFolder() ) );
	QDir().mkdir( make_qt( scone::GetSettingsFolder() / "log" ) );

	xo::path log_file = scone::GetSettingsFolder() / "log" / xo::path( xo::get_date_time_str( "%Y-%m-%d_%H%M%S" ) + ".txt" );
	xo::log::file_sink file_sink( xo::log::debug_level, log_file );

	if ( !file_sink.good() )
	{
		QMessageBox::critical( 0, "Error creating log file", "Could not create file " + make_qt( log_file.str() ) );
		return -1;
	}
	else xo::log::debug( "Created log file ", log_file );

#ifdef _DEBUG
	xo::log::stream_sink console_log_sink( xo::log::trace_level, std::cout );
#endif

	try
	{
		// init plash screen
		QPixmap splash_pm( make_qt( scone::GetFolder( scone::SCONE_UI_RESOURCE_FOLDER ) / "scone_splash.png" ) );
		QSplashScreen splash( splash_pm );
		splash.show();
		//splash.showMessage( QString( "Initiating SCONE version " ) + scone::GetSconeVersion().to_str().c_str(), Qt::AlignLeft | Qt::AlignBaseline, Qt::black);
		a.processEvents();

		// init main window
		SconeStudio w;

		// sleep a while so people can enjoy the splash screen :-)
		QThread::sleep( 0 );

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
		scone::log::info( "SCONE version ", scone::GetSconeVersion() );

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
