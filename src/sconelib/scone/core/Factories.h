#pragma once

#include "xo/utility/factory.h"

#include "scone/core/PropNode.h"
#include "scone/core/types.h"

#include "scone/optimization/Params.h"

#include "scone/controllers/Reflex.h"
#include "scone/model/Model.h"
#include "scone/model/Location.h"

namespace scone
{
	SCONE_API ControllerUP CreateController( const PropNode& props, Params& par, Model& model, const Location& target_area );
	SCONE_API MeasureUP CreateMeasure( const PropNode& props, Params& par, Model& model, const Location& target_area );
	SCONE_API ReflexUP CreateReflex( const PropNode& props, Params& par, Model& model, const Location& target_area );
	SCONE_API FunctionUP CreateFunction( const PropNode& props, Params& par );

	SCONE_API SensorUP CreateSensor( const PropNode&, Params&, Model&, const Location& );
	SCONE_API ModelUP CreateModel( const PropNode& prop, Params& par );

	SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop );
	SCONE_API ObjectiveUP CreateObjective( const PropNode& prop );
	SCONE_API xo::factory< Objective, const PropNode& >& GetObjectiveFactory();
}
