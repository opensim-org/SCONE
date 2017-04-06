#include "common.h"

#include <boost/filesystem.hpp>
#include <boost/predef.h>
#include "platform.h"

using namespace boost::filesystem;
using std::cout;
using std::endl;

BOOST_AUTO_TEST_CASE( simulation_test )
{
	path testpath = ( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "unittestdata/simulation_test" ).str();

	for ( directory_iterator dir_it( testpath ); dir_it != directory_iterator(); ++dir_it )
	{
		if ( is_directory( dir_it->path() ) )
		{
			for ( directory_iterator fileit( dir_it->path() ); fileit != directory_iterator(); ++fileit )
			{
				//cout << "Checking file " << fileit->path().string() << endl;
				if ( fileit->path().extension() == ".par" )
				{
					path fp = fileit->path();
					flut::prop_node result = scone::RunSimulation( fp.string() );

					path reportpath = fp.parent_path() / ( "result_" + make_platform_id() + "_" + fp.stem().string() + ".prop" );
					if ( !exists( reportpath ) )
					{
						BOOST_ERROR( "Could not find simulation report: " + reportpath.string() );
						save_prop( result, reportpath.string() );
					}
					else
					{
						auto verify = flut::load_prop( reportpath.string() );
						auto rep1 = result.get_child( "result" );
						auto rep2 = verify.get_child( "result" );
						BOOST_CHECK( rep1 == rep2 );
						if ( rep1 != rep2 )
						{
							BOOST_TEST_MESSAGE( "rep1:" );
							BOOST_TEST_MESSAGE( rep1 );
							BOOST_TEST_MESSAGE( "rep2:" );
							BOOST_TEST_MESSAGE( rep2 );
						}
					}
				}
			}
		}
	}
}
