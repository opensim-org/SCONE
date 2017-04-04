#pragma once

#include "flut/factory.hpp"

#include "scone/core/PropNode.h"
#include "scone/core/Function.h"
#include "scone/controllers/cs_fwd.h"
#include "scone/optimization/ParamSet.h"
#include "scone/model/sim_fwd.h"

#include "scone/controllers/Reflex.h"
#include "scone/model/Model.h"
#include "scone/model/Area.h"

namespace scone
{
	SCONE_API sim::ControllerUP CreateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
	SCONE_API ReflexUP CreateReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
	SCONE_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& par );

	SCONE_API sim::SensorUP CreateSensor( const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& );
	SCONE_API sim::ModelUP CreateModel( const PropNode& prop, opt::ParamSet& par );

	SCONE_API opt::OptimizerUP CreateOptimizer( const PropNode& prop );
	SCONE_API opt::ObjectiveUP CreateObjective( const PropNode& prop, opt::ParamSet& par );
	SCONE_API flut::factory< opt::Objective, const PropNode&, opt::ParamSet& >& GetObjectiveFactory();
}
