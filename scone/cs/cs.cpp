#include "stdafx.h"
#include "cs.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "../sim/simbody/sim_simbody.h"
#include <OpenSim/OpenSim.h>

#include "../opt/Optimizer.h"

#include "SimulationObjective.h"
#include "FeedForwardController.h"
#include "HeightMeasure.h"
#include "GaitMeasure.h"
#include "EnergyMeasure.h"
#include "../sim/Factories.h"
#include "../sim/simbody/Model_Simbody.h"
#include "../opt/Factories.h"

namespace scone
{
	namespace cs
	{
		void CS_API RegisterFactoryTypes()
		{
			// simulation engines
			sim::RegisterSimbody();

			// register objective
			opt::GetObjectiveFactory().Register< SimulationObjective >();

			// register controllers
			sim::GetControllerFactory().Register< FeedForwardController >();
			sim::GetControllerFactory().Register< HeightMeasure >();
			sim::GetControllerFactory().Register< GaitMeasure >();
			sim::GetControllerFactory().Register< EnergyMeasure >();
		}

		void CS_API PerformOptimization( const String& config_file )
		{
			RegisterFactoryTypes();

			// copy config file to output folder
			opt::OptimizerUP opt = opt::CreateOptimizerFromXml( config_file );
			copy_file( path( config_file ), path( opt->GetOutputFolder() ) / path( config_file ).filename(), copy_option::overwrite_if_exists );

			// start the optimization
			opt->Run();
		}

	}
}
