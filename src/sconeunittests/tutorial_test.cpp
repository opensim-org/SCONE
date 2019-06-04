#include <filesystem>
#include "xo/filesystem/path.h"
#include "scone/core/system_tools.h"
#include "xo/system/log.h"
#include "scone/optimization/opt_tools.h"
#include "xo/system/test_framework.h"
#include "xo/filesystem/filesystem.h"

namespace fs = std::filesystem;

XO_TEST_CASE( tutorial_test )
{
	auto scone_dir = scone::GetFolder( scone::SCONE_ROOT_FOLDER );
	auto tutorials_dir = scone_dir / "scenarios/Tutorials";
	auto results_dir = scone_dir / "resources/unittestdata/tutorial_test" / xo::get_computer_name() + "_results";
	xo::create_directories( results_dir );

	for ( fs::directory_iterator fileit( tutorials_dir.str() ); fileit!= fs::directory_iterator(); ++fileit )
	{
		auto scenario_file = xo::path( fileit->path().string() );
		if ( scenario_file.extension() == "scone" )
		{
			xo::log::info( "TESTING: ", scenario_file.filename() );
			xo::path report_file = results_dir / scenario_file.stem() + ".zml";
			auto scenario_pn = xo::load_file_with_include( scenario_file, "INCLUDE" );
			auto result_pn = scone::EvaluateScenario( scenario_pn, scenario_file, xo::path() );

			if ( xo::file_exists( report_file ) )
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
			else
			{
				xo::log::warning( "Could not find results for ", report_file.filename() );
				xo::save_file( result_pn, report_file );
			}
		}
	}
}
