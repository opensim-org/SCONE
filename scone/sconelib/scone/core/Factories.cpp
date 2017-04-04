#include "Factories.h"
#include "flut/factory.hpp"
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
#include "scone/optimization/CmaOptimizerShark3.h"
#include "scone/optimization/CmaOptimizerCCMAES.h"
#include "scone/objectives/SimulationObjective.h"

namespace scone
{
	static flut::factory< sim::Controller, const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& > g_ControllerFactory;
	static flut::factory< Reflex, const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& > g_ReflexFactory;
	static flut::factory< Function, const PropNode&, opt::ParamSet& > g_FunctionFactory;

	SCONE_API sim::ControllerUP CreateController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area )
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
		}
		return g_ControllerFactory( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API ReflexUP CreateReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area )
	{
		if ( g_ReflexFactory.empty() )
		{
			g_ReflexFactory.register_class< MuscleReflex >();
			g_ReflexFactory.register_class< DofReflex >();
			g_ReflexFactory.register_class< ConditionalMuscleReflex >();
		}
		return g_ReflexFactory( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API FunctionUP CreateFunction( const PropNode& props, opt::ParamSet& par )
	{
		if ( g_FunctionFactory.empty() )
		{
			g_FunctionFactory.register_class< PieceWiseConstantFunction >();
			g_FunctionFactory.register_class< PieceWiseLinearFunction >();
			g_FunctionFactory.register_class< Polynomial >();
		}
		return g_FunctionFactory( props.get< String >( "type" ), props, par );
	}

	static flut::factory< sim::Model, const PropNode&, opt::ParamSet& > g_ModelFactory;
	SCONE_API sim::ModelUP CreateModel( const PropNode& prop, opt::ParamSet& par )
	{
		if ( g_ModelFactory.empty() )
		{
			g_ModelFactory.register_class< sim::Model_Simbody >( "Simbody" );
		}
		return g_ModelFactory( prop.get< String >( "type" ), prop, par );
	}

	static flut::factory< sim::Sensor, const PropNode&, opt::ParamSet&, sim::Model&, const sim::Area& > g_SensorFactory;
	SCONE_API sim::SensorUP CreateSensor( const PropNode& props, opt::ParamSet& par, sim::Model& m, const sim::Area& a )
	{
		if ( g_SensorFactory.empty() )
		{
			g_SensorFactory.register_class< sim::MuscleForceSensor >();
			g_SensorFactory.register_class< sim::MuscleLengthSensor >();
			g_SensorFactory.register_class< sim::MuscleVelocitySensor >();
			g_SensorFactory.register_class< sim::MuscleSpindleSensor >();
			g_SensorFactory.register_class< sim::DofPositionSensor >();
			g_SensorFactory.register_class< sim::DofVelocitySensor >();
		}
		return g_SensorFactory( props.get< String >( "type" ), props, par, m, a );
	}

	static flut::factory< opt::Optimizer, const PropNode& > g_OptimizerFactory;
	SCONE_API opt::OptimizerUP CreateOptimizer( const PropNode& prop )
	{
		if ( g_OptimizerFactory.empty() )
		{
			g_OptimizerFactory.register_class< opt::CmaOptimizerCCMAES >( "CmaOptimizer" );
			g_OptimizerFactory.register_class< opt::CmaOptimizerShark3 >();
			g_OptimizerFactory.register_class< opt::CmaOptimizerCCMAES >();
		}
		return g_OptimizerFactory( prop.get< String >( "type" ), prop );
	}

	static flut::factory< opt::Objective, const PropNode&, opt::ParamSet& > g_ObjectiveFactory;
	SCONE_API flut::factory< opt::Objective, const PropNode&, opt::ParamSet& >& GetObjectiveFactory()
	{
		if ( g_ObjectiveFactory.empty() )
		{
			g_ObjectiveFactory.register_class< SimulationObjective >();
		}
		return g_ObjectiveFactory;
	}

	SCONE_API opt::ObjectiveUP CreateObjective( const PropNode& prop, opt::ParamSet& par )
	{
		return GetObjectiveFactory()( prop.get< String >( "type" ), prop, par );
	}
}
