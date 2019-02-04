#include "xo/filesystem/path.h"
#include "scone/core/system_tools.h"
#include <filesystem>

namespace fs = std::filesystem;

void simulation_test()
{
	fs::path testpath = ( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "unittestdata/simulation_test" ).string();

	XO_NOT_IMPLEMENTED;

	for ( fs::directory_iterator dir_it( testpath ); dir_it != fs::directory_iterator(); ++dir_it )
	{
		if ( fs::is_directory( dir_it->path() ) )
		{
			for ( fs::directory_iterator fileit( dir_it->path() ); fileit != fs::directory_iterator(); ++fileit )
			{
				//cout << "Checking file " << fileit->path().string() << endl;
				fs::path fp = fileit->path();
				if ( fileit->path().extension() == ".par" )
				{
					xo::path scenario_file = FindScenario( xo::path( fp.string() ) );
					auto scenario_pn = xo::load_file_with_include( scenario_file, "INCLUDE" );

					xo::prop_node result = scone::EvaluateScenario( scenario_pn, xo::path( fp.string() ), xo::path( fp.parent_path().string() ) );

					fs::path reportpath = fp.parent_path() / ( "result_" + fp.stem().string() + ".prop" );
					if ( !exists( reportpath ) )
					{
						xo::save_file( result, xo::path( reportpath.string() ) );
						xo_error( "Could not find simulation report: " + reportpath.string() );
					}
					else
					{
						auto verify = xo::load_file( xo::path( reportpath.string() ) );
						auto rep1 = result.get_child( "result" );
						auto rep2 = verify.get_child( "result" );
						XO_TEST( rep1 == rep2 );
						if ( rep1 != rep2 )
						{
							log::error( "rep1:", rep1 );
							log::error( "rep2:", rep2 );
						}
					}
				}
			}
		}
	}
}
