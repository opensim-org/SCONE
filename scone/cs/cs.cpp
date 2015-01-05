#include "stdafx.h"
#include "cs.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "../sim/simbody/sim_simbody.h"
#include <OpenSim/OpenSim.h>

#include "../opt/Optimizer.h"

#include "SimulationObjective.h"
#include "FeedForwardController.h"
#include "JumpingMeasure.h"

namespace scone
{
	namespace cs
	{
		void CS_API RegisterFactoryTypes()
		{
			// simulation engines
			sim::RegisterSimbody();

			// cs types
			SimulationObjective::RegisterFactory();
			FeedForwardController::RegisterFactory();
			JumpingMeasure::RegisterFactory();
		}

		void CS_API PerformOptimization( const String& config_file )
		{
			RegisterFactoryTypes();

			// copy config file to output folder
			opt::OptimizerUP opt = opt::CreateOptimizerFromXml( config_file );
			copy_file( path( config_file ), path( opt->GetOutputFolder() ) / path( config_file ).filename() );

			// start the optimization
			opt->Run();
		}

	}
}
