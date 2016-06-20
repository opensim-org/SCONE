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


		// set log level to trace in debug mode
#ifdef _DEBUG
		log::SetLevel( log::TraceLevel );
#endif

		// register all types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();
		sim::RegisterSimbody();

		if ( fileName.extension().string() == ".xml" )
		{
			// if .xml file given, do an optimization
			opt::PerformOptimization( argc, argv );
		}
		else if ( fileName.extension().string() == ".par" )
		{
			// if .par file given, evaluate the objective and print log
            log::SetLevel( log::TraceLevel );
			opt::SimulateObjective( fileName.string() );
		}
		else
		{
			// Otherwise, not sure what to do.
			SCONE_THROW( "Unknown file type (must be .xml or .par): " + fileName.string() );
		}
	}
	catch (std::exception& e)
	{
		log::Critical( e.what() );
		flut::wait_for_key();
	}

	return 0;
}
