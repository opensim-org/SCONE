#include <filesystem>

#include "scone/core/system_tools.h"
#include "scone/optimization/opt_tools.h"
#include "xo/filesystem/filesystem.h"
#include "xo/filesystem/path.h"
#include "xo/serialization/serialize.h"
#include "xo/system/log.h"
#include "xo/system/test_case.h"

namespace fs = std::filesystem;

namespace xo
{
	namespace test
	{
		struct scenario_test {
			scenario_test( const path& scenario_file, const path& report_file ) :
				scenario_file_( scenario_file ),
				report_file_( report_file )
			{}

			void operator()( test_case& XO_ACTIVE_TEST_CASE ) {
				auto scenario_pn = xo::load_file_with_include( scenario_file_, "INCLUDE" );
				auto eval_pn = scone::EvaluateScenario( scenario_pn, scenario_file_, xo::path() );

				if ( xo::file_exists( report_file_ ) )
				{
					auto base_pn = xo::load_file( report_file_ );
					auto eval_result = eval_pn.get_child( "result" );
					auto base_result = base_pn.get_child( "result" );
					XO_CHECK( eval_result == base_result );
					if ( eval_result != base_result )
					{
						xo::log::info( "baseline:\n", base_pn );
						xo::log::info( "evaluated:\n", eval_pn );
					}
				}
				else
				{
					XO_CHECK_MESSAGE( false, "Could not find results for " + report_file_.filename().str() );
					xo::save_file( eval_pn, report_file_ );
				}
			}

			path scenario_file_;
			path report_file_;
		};
	}
}

namespace scone
{
	void add_scenario_tests( const path& scenario_dir )
	{
		auto scone_dir = GetFolder( SCONE_ROOT_FOLDER );
		auto tutorials_dir = scone_dir / scenario_dir;
		auto results_dir = scone_dir / "resources/unittestdata" / scenario_dir.filename() / xo::get_computer_name() + "_results";
		xo::create_directories( results_dir );

		for ( fs::directory_iterator fileit( tutorials_dir.str() ); fileit != fs::directory_iterator(); ++fileit )
		{
			auto scenario_file = xo::path( fileit->path().string() );
			if ( scenario_file.extension_no_dot() == "scone" )
			{
				xo::log::debug( "Adding test case: ", scenario_file.filename() );
				xo::path report_file = results_dir / scenario_file.stem() + ".zml";
				xo::test::add_test_case( scenario_file.stem().str(), xo::test::scenario_test( scenario_file, report_file ) );
			}
		}
	}
}
