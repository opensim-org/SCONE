#include "scone/core/Log.h"
#include "scone/opt/opt.h"
#include "scone/cs/cs.h"
#include "scone/sim/simbody/sim_simbody.h"
#include "flut/system_tools.hpp"
#include <boost/filesystem.hpp>

using namespace scone;
using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		SCONE_THROW_IF( argc < 2, "No file argument provided" );

		boost::filesystem::path fileName(argv[1]);
		SCONE_THROW_IF( !boost::filesystem::exists( fileName ), "File does not exist");

		// If .xml file given, do an optimization
		if ( fileName.extension().string() == ".xml" )
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
		
		// If .par file given, evaluate the objective
		else if ( fileName.extension().string() == ".par" )
		{
			opt::SimulateObjective( fileName.string() );
		}

		// Otherwise, not sure what to do.
		else
		{
			SCONE_THROW( "Must give a .xml or .par file" );
		}

	}
	catch (std::exception& e)
	{
		log::Critical( e.what() );
		flut::wait_for_key();
	}

	return 0;
}
