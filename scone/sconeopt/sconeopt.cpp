#include "scone/core/Log.h"
#include "scone/opt/opt.h"
#include "scone/cs/cs.h"
#include "scone/sim/simbody/sim_simbody.h"
#include "flut/system_tools.hpp"

using namespace scone;
using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		// set log level to trace in debug mode
#ifdef _DEBUG
		log::SetLevel( log::TraceLevel );
#endif

		// register all types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();
		sim::RegisterSimbody();

		// perform the optimization
		opt::PerformOptimization( argc, argv );
	}
	catch (std::exception& e)
	{
		log::Critical( e.what() );
		flut::wait_for_key();
	}

	return 0;
}
