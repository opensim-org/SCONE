#include "Factories.h"
#include "xo/utility/factory.h"
#include "scone/controllers/GaitStateController.h"
#include "scone/controllers/TimeStateController.h"
#include "scone/controllers/PerturbationController.h"
#include "scone/controllers/FeedForwardController.h"
#include "scone/controllers/ReflexController.h"
#include "scone/controllers/MetaReflexController.h"
#include "scone/objectives/GaitMeasure.h"
#include "scone/objectives/GaitCycleMeasure.h"
#include "scone/objectives/CompositeMeasure.h"
#include "scone/objectives/JumpMeasure.h"
#include "scone/objectives/JointLoadMeasure.h"
#include "scone/objectives/ReactionForceMeasure.h"
#include "scone/objectives/PointMeasure.h"
#include "scone/objectives/HeightMeasure.h"
#include "scone/model/Sensors.h"
#include "scone/controllers/MuscleReflex.h"
#include "scone/controllers/DofReflex.h"
#include "scone/controllers/ConditionalMuscleReflex.h"
#include "scone/core/PieceWiseConstantFunction.h"
#include "scone/core/PieceWiseLinearFunction.h"
#include "scone/core/Polynomial.h"
#include "scone/model/simbody/Model_Simbody.h"
#include "scone/optimization/CmaOptimizerCCMAES.h"
#include "scone/objectives/SimulationObjective.h"
#include "scone/controllers/SensorStateController.h"
#include "scone/controllers/MirrorController.h"
#include "scone/controllers/NeuralController.h"
#include "../objectives/ImitationObjective.h"
#include "../objectives/BalanceMeasure.h"
#include "../objectives/SimilarityObjective.h"

namespace scone
{
	static xo::factory< Controller, const PropNode&, Params&, Model&, const Locality& > g_ControllerFactory;
	static xo::factory< Reflex, const PropNode&, Params&, Model&, const Locality& > g_ReflexFactory;
	static xo::factory< Function, const PropNode&, Params& > g_FunctionFactory;

	SCONE_API ControllerUP CreateController( const PropNode& props, Params& par, Model& model, const Locality& target_area )
	{
		if ( g_ControllerFactory.empty() )
		{
			// register controllers
			g_ControllerFactory.register_class< FeedForwardController >();
			g_ControllerFactory.register_class< GaitStateController >();
			g_ControllerFactory.register_class< ReflexController >();
			g_ControllerFactory.register_class< TimeStateController >();
			g_ControllerFactory.register_class< MetaReflexController >();
			g_ControllerFactory.register_class< PerturbationController >();
			g_ControllerFactory.register_class< SensorStateController >();
			g_ControllerFactory.register_class< MirrorController >();
			g_ControllerFactory.register_class< NeuralController >();

			// register measures
			g_ControllerFactory.register_class< HeightMeasure >();
			g_ControllerFactory.register_class< GaitMeasure >();
			g_ControllerFactory.register_class< GaitCycleMeasure >();
			g_ControllerFactory.register_class< EffortMeasure >();
			g_ControllerFactory.register_class< DofLimitMeasure >();
			g_ControllerFactory.register_class< CompositeMeasure >();
			g_ControllerFactory.register_class< JumpMeasure >();
			g_ControllerFactory.register_class< JointLoadMeasure >();
			g_ControllerFactory.register_class< ReactionForceMeasure >();
			g_ControllerFactory.register_class< PointMeasure >();
			g_ControllerFactory.register_class< BalanceMeasure >();
		}
		return g_ControllerFactory( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API ReflexUP CreateReflex( const PropNode& props, Params& par, Model& model, const Locality& target_area )
	{
		if ( g_ReflexFactory.empty() )
		{
			g_ReflexFactory.register_class< MuscleReflex >();
			g_ReflexFactory.register_class< DofReflex >();
			g_ReflexFactory.register_class< ConditionalMuscleReflex >();
		}
		return g_ReflexFactory( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API FunctionUP CreateFunction( const PropNode& props, Params& par )
	{
		if ( g_FunctionFactory.empty() )
		{
			g_FunctionFactory.register_class< PieceWiseConstantFunction >();
			g_FunctionFactory.register_class< PieceWiseLinearFunction >();
			g_FunctionFactory.register_class< Polynomial >();
		}
		return g_FunctionFactory( props.get< String >( "type" ), props, par );
	}

	static xo::factory< Model, const PropNode&, Params& > g_ModelFactory;
	SCONE_API ModelUP CreateModel( const PropNode& prop, Params& par )
	{
		if ( g_ModelFactory.empty() )
		{
			g_ModelFactory.register_class< Model_Simbody >( "Simbody" );
		}
		return g_ModelFactory( prop.get< String >( "type" ), prop, par );
	}

	static xo::factory< Sensor, const PropNode&, Params&, Model&, const Locality& > g_SensorFactory;
	SCONE_API SensorUP CreateSensor( const PropNode& props, Params& par, Model& m, const Locality& a )
	{
		if ( g_SensorFactory.empty() )
		{
			g_SensorFactory.register_class< MuscleForceSensor >();
			g_SensorFactory.register_class< MuscleLengthSensor >();
			g_SensorFactory.register_class< MuscleVelocitySensor >();
			g_SensorFactory.register_class< MuscleSpindleSensor >();
			g_SensorFactory.register_class< DofPositionSensor >();
			g_SensorFactory.register_class< DofVelocitySensor >();
		}
		return g_SensorFactory( props.get< String >( "type" ), props, par, m, a );
	}

	static xo::factory< Optimizer, const PropNode& > g_OptimizerFactory;
	SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop )
	{
		if ( g_OptimizerFactory.empty() )
		{
			g_OptimizerFactory.register_class< CmaOptimizerCCMAES >( "CmaOptimizer" );
			g_OptimizerFactory.register_class< CmaOptimizerCCMAES >();
		}
		return g_OptimizerFactory( prop.get< String >( "type" ), prop );
	}

	static xo::factory< Objective, const PropNode& > g_ObjectiveFactory;
	SCONE_API xo::factory< Objective, const PropNode& >& GetObjectiveFactory()
	{
		if ( g_ObjectiveFactory.empty() )
		{
			g_ObjectiveFactory.register_class< SimulationObjective >();
			g_ObjectiveFactory.register_class< ImitationObjective >();
			g_ObjectiveFactory.register_class< SimilarityObjective >();
		}
		return g_ObjectiveFactory;
	}

	SCONE_API ObjectiveUP CreateObjective( const PropNode& prop )
	{
		return GetObjectiveFactory()( prop.get< String >( "type" ), prop );
	}
}
