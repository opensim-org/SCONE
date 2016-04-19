#include "common.h"

#include "scone/cs/tools.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

BOOST_AUTO_TEST_CASE( simulation_test )
{
	path rootpath = scone::GetSconeFolder( "root" );
	auto result = scone::RunSimulation( ( rootpath / "unittestdata/GHW15/85.518.par" ).string() );

	std::cout << result.GetChild( "report" );
}
