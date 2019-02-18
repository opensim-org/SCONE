/*
** main.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SconeStudio.h"
#include <QtCore/QtGlobal>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtWidgets/QApplication>
    #include <QtWidgets/QMessageBox.h>
#else
    #include <QtGui/QApplication>
#endif

#include "scone/core/system_tools.h"
#include "scone/core/string_tools.h"
#include "scone/core/Log.h"

#include "qt_tools.h"

#include "xo/system/log_sink.h"
#include "xo/system/system_tools.h"
#include "xo/filesystem/filesystem.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "scone/core/version.h"

#include "scone/sconelib_config.h"
#include "scone/core/Exception.h"

int main( int argc, char *argv[] )
{
	QApplication::setStyle( "fusion" );
	QApplication a( argc, argv );
	QCoreApplication::setAttribute( Qt::AA_UseDesktopOpenGL );

	try
	{
		// init plash screen
		QPixmap splash_pm( make_qt( scone::GetFolder( scone::SCONE_UI_RESOURCE_FOLDER ) / "scone_splash.png" ) );
		QSplashScreen splash( splash_pm );
		splash.show();
		a.processEvents();

		// init logging
		xo::path log_file = scone::GetSettingsFolder() / "log" / xo::path( xo::get_date_time_str( "%Y%m%d_%H%M%S" ) + ".log" );
		xo::log::file_sink file_sink( xo::log::debug_level, log_file );
		SCONE_THROW_IF( !file_sink.good(), "Could not create file " + log_file.string() );
		xo::log::debug( "Created log file ", log_file );
#ifdef _DEBUG
		xo::log::stream_sink console_log_sink( xo::log::trace_level, std::cout );
#endif

		// init scone file format
		xo::register_serializer< xo::prop_node_serializer_zml >( "scone" );

		// register models
		scone::RegisterModels();

		// init main window
		SconeStudio w;
		QThread::sleep( 0 ); // sleep a while so people can enjoy the splash screen :-)
		w.init();
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

#ifdef _WIN32
#ifndef DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return main( __argc, __argv );
}
#endif
#endif
