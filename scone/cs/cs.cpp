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
#include "EffortMeasure.h"
#include "../sim/Factories.h"
#include "../sim/simbody/Model_Simbody.h"
#include "../opt/Factories.h"
#include "Factories.h"
#include "PieceWiseConstantFunction.h"
#include "PieceWiseLinearFunction.h"
#include "Polynomial.h"
#include "GaitStateController.h"

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
			sim::GetControllerFactory().Register< GaitStateController >();
			sim::GetControllerFactory().Register< HeightMeasure >();
			sim::GetControllerFactory().Register< GaitMeasure >();
			sim::GetControllerFactory().Register< EffortMeasure >();

			// register functions
			GetFunctionFactory().Register< PieceWiseConstantFunction >();
			GetFunctionFactory().Register< PieceWiseLinearFunction >();
			GetFunctionFactory().Register< Polynomial >();
		}
	}
}
