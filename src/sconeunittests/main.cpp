#include "optimization_test.h"
#include "simulation_test.h"
#include "scone/sconelib_config.h"
#include "xo/serialization/serialize.h"

int main( int argc, const char* argv[] )
{
	xo::log::console_sink sink( xo::log::info_level );
	xo::register_serializer< xo::prop_node_serializer_zml >( "scone" );
	scone::RegisterModels();

	try
	{
		optimization_test();
		simulation_test();
	}
	catch ( std::exception& e)
	{
		log::critical( "Could not perform tests: ", e.what() );
		return 1;
	}

	return XO_TEST_REPORT();
}
