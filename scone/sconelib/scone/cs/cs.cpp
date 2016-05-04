#include "cs.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

#include "scone/opt/Optimizer.h"

#include "SimulationObjective.h"
#include "FeedForwardController.h"
#include "HeightMeasure.h"
#include "GaitMeasure.h"
#include "EffortMeasure.h"
#include "scone/sim/Factories.h"
#include "scone/opt/Factories.h"
#include "Factories.h"
#include "PieceWiseConstantFunction.h"
#include "PieceWiseLinearFunction.h"
#include "Polynomial.h"
#include "GaitStateController.h"
#include "ReflexController.h"
#include "DofLimitMeasure.h"
#include "CompositeMeasure.h"
#include "GaitCycleMeasure.h"
#include "DofReflex.h"
#include "MuscleReflex.h"
#include "ConditionalMuscleReflex.h"
#include "JumpMeasure.h"
#include "TimeStateController.h"
#include "MetaReflexController.h"

namespace scone
{
	namespace cs
	{
		void SCONE_API RegisterFactoryTypes()
		{
			// register sim factory types
			sim::RegisterFactoryTypes();

			// register objective
			opt::GetObjectiveFactory().Register< SimulationObjective >();

			// register controllers
			sim::GetControllerFactory().Register< FeedForwardController >();
			sim::GetControllerFactory().Register< GaitStateController >();
			sim::GetControllerFactory().Register< ReflexController >();
			sim::GetControllerFactory().Register< TimeStateController >();
			sim::GetControllerFactory().Register< MetaReflexController >();

			// register reflexes
			// TODO: make this not separate but use controller factory instead?
			GetReflexFactory().Register< MuscleReflex >();
			GetReflexFactory().Register< DofReflex >();
			GetReflexFactory().Register< ConditionalMuscleReflex >();

			// register measures
			sim::GetControllerFactory().Register< HeightMeasure >();
			sim::GetControllerFactory().Register< GaitMeasure >();
			sim::GetControllerFactory().Register< GaitCycleMeasure >();
			sim::GetControllerFactory().Register< EffortMeasure >();
			sim::GetControllerFactory().Register< DofLimitMeasure >();
			sim::GetControllerFactory().Register< CompositeMeasure >();
			sim::GetControllerFactory().Register< JumpMeasure >();

			// register functions
			GetFunctionFactory().Register< PieceWiseConstantFunction >();
			GetFunctionFactory().Register< PieceWiseLinearFunction >();
			GetFunctionFactory().Register< Polynomial >();
		}
	}
}
