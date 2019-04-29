/*
** simulation_test.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include <filesystem>
#include "xo/filesystem/path.h"
#include "scone/core/system_tools.h"
#include "xo/system/log.h"
#include "scone/optimization/opt_tools.h"
#include "xo/diagnose/test_framework.h"

namespace fs = std::filesystem;

XO_TEST_CASE( simulation_test )
{
	fs::path testpath = ( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "resources/unittestdata/simulation_test" ).str();

	for ( fs::directory_iterator dir_it( testpath ); dir_it != fs::directory_iterator(); ++dir_it )
	{
		if ( fs::is_directory( dir_it->path() ) )
		{
			for ( fs::directory_iterator fileit( dir_it->path() ); fileit != fs::directory_iterator(); ++fileit )
			{
				fs::path parfile = fileit->path();
				if ( parfile.extension() == ".par" )
				{
					xo::log::debug("Checking ", parfile.string());
					xo::path scenario_file = scone::FindScenario( xo::path( parfile.string() ) );
					fs::path base_report_file = parfile.parent_path() / ( "simulation_test_result_" + parfile.stem().string() + ".zml" );
					auto scenario_pn = xo::load_file_with_include( scenario_file, "INCLUDE" );
					auto result_pn = scone::EvaluateScenario( scenario_pn, parfile, xo::path() );

					if ( !exists( base_report_file ) )
					{
						xo::log::warning( "Could not find: ", base_report_file.string() );
						xo::save_file( result_pn, base_report_file );
					}
					else
					{
						auto base_report_pn = xo::load_file( base_report_file );
						auto rep1 = result_pn.get_child( "result" );
						auto rep2 = base_report_pn.get_child( "result" );
						XO_CHECK( rep1 == rep2 );
						if ( rep1 != rep2 )
						{
							xo::log::error( "baseline:\n", rep1 );
							xo::log::error( "test result:\n", rep2 );
						}
					}
				}
			}
		}
	}
}
