#pragma once

#include "flut/factory.hpp"

#include "scone/core/PropNode.h"
#include "scone/core/types.h"

#include "scone/optimization/ParamSet.h"

#include "scone/controllers/Reflex.h"
#include "scone/model/Model.h"
#include "scone/model/Area.h"

namespace scone
{
	SCONE_API ControllerUP CreateController( const PropNode& props, ParamSet& par, Model& model, const Area& target_area );
	SCONE_API ReflexUP CreateReflex( const PropNode& props, ParamSet& par, Model& model, const Area& target_area );
	SCONE_API FunctionUP CreateFunction( const PropNode& props, ParamSet& par );

	SCONE_API SensorUP CreateSensor( const PropNode&, ParamSet&, Model&, const Area& );
	SCONE_API ModelUP CreateModel( const PropNode& prop, ParamSet& par );

	SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop );
	SCONE_API ObjectiveUP CreateObjective( const PropNode& prop, ParamSet& par );
	SCONE_API flut::factory< Objective, const PropNode&, ParamSet& >& GetObjectiveFactory();
}
