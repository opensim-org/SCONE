#include "optimization_test.h"
#include "propnode_test.h"
#include "simulation_test.h"

int main( int argc, const char* argv[] )
{
	xo::log::console_sink sink( xo::log::info_level );

	try
	{
		optimization_test();
		propnode_stream_test();
	}
	catch ( std::exception& e)
	{
		log::critical( "Could not perform tests: ", e.what() );
		return 1;
	}

	return XO_TEST_REPORT();
}
