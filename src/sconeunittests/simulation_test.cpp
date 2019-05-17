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
#include "xo/filesystem/filesystem.h"

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
				fs::path fsparfile = fileit->path();
				if ( fsparfile.extension() == ".par" )
				{
					xo::path parfile( fsparfile.string() );
					xo::log::debug("Checking ", parfile );
					xo::path scenario_file = scone::FindScenario( parfile );
					std::string result_prefix = xo::get_computer_name() + "_simulation_test_result_";
					xo::path report_file = parfile.parent_path() / ( result_prefix + parfile.stem().str() + ".zml" );
					auto scenario_pn = xo::load_file_with_include( scenario_file, "INCLUDE" );
					auto result_pn = scone::EvaluateScenario( scenario_pn, parfile, xo::path() );

					if ( !xo::file_exists( report_file ) )
					{
						xo::log::warning( "Could not find: ", report_file );
						xo::save_file( result_pn, report_file );
					}
					else
					{
						auto base_report_pn = xo::load_file( report_file );
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
