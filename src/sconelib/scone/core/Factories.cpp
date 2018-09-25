#include "Factories.h"
#include "xo/utility/factory.h"
#include "scone/controllers/GaitStateController.h"
#include "scone/controllers/TimeStateController.h"
#include "scone/controllers/PerturbationController.h"
#include "scone/controllers/FeedForwardController.h"
#include "scone/controllers/ReflexController.h"
#include "scone/measures/GaitMeasure.h"
#include "scone/measures/GaitCycleMeasure.h"
#include "scone/measures/CompositeMeasure.h"
#include "scone/measures/JumpMeasure.h"
#include "scone/measures/JointLoadMeasure.h"
#include "scone/measures/ReactionForceMeasure.h"
#include "scone/measures/PointMeasure.h"
#include "scone/measures/HeightMeasure.h"
#include "scone/model/Sensors.h"
#include "scone/controllers/MuscleReflex.h"
#include "scone/controllers/DofReflex.h"
#include "scone/controllers/ConditionalMuscleReflex.h"
#include "scone/core/PieceWiseConstantFunction.h"
#include "scone/core/PieceWiseLinearFunction.h"
#include "scone/core/Polynomial.h"
#include "scone/model/simbody/ModelSimbody.h"
#include "scone/optimization/CmaOptimizerCCMAES.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/controllers/SensorStateController.h"
#include "scone/controllers/MirrorController.h"
#include "scone/controllers/NeuralController.h"
#include "scone/optimization/ImitationObjective.h"
#include "scone/measures/BalanceMeasure.h"
#include "scone/optimization/SimilarityObjective.h"
#include "scone/optimization/CmaOptimizerSpot.h"
#include "scone/controllers/CompositeController.h"
#include "../measures/MimicMeasure.h"
#include "../optimization/CmaPoolOptimizer.h"
#include "../optimization/TestObjective.h"
#include "../controllers/SequentialController.h"
#include "../measures/DofMeasure.h"
#include "../measures/SymmetricMeasure.h"
#include "../measures/BodyMeasure.h"

namespace scone
{
	SCONE_API ControllerUP CreateController( const PropNode& props, Params& par, Model& model, const Location& target_area )
	{
		static xo::factory< Controller, const PropNode&, Params&, Model&, const Location& > g_ControllerFactory;
		if ( g_ControllerFactory.empty() )
		{
			// register controllers
			g_ControllerFactory.register_class< FeedForwardController >();
			g_ControllerFactory.register_class< GaitStateController >();
			g_ControllerFactory.register_class< ReflexController >();
			g_ControllerFactory.register_class< TimeStateController >();
			g_ControllerFactory.register_class< PerturbationController >();
			g_ControllerFactory.register_class< SensorStateController >();
			g_ControllerFactory.register_class< MirrorController >();
			g_ControllerFactory.register_class< NeuralController >();
			g_ControllerFactory.register_class< CompositeController >();
			g_ControllerFactory.register_class< SequentialController >();

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

	SCONE_API MeasureUP CreateMeasure( const PropNode& props, Params& par, Model& model, const Location& target_area )
	{
		static xo::factory< Measure, const PropNode&, Params&, Model&, const Location& > g_MeasureFactory;
		if ( g_MeasureFactory.empty() )
		{
			// register measures
			g_MeasureFactory.register_class< HeightMeasure >();
			g_MeasureFactory.register_class< GaitMeasure >();
			g_MeasureFactory.register_class< GaitCycleMeasure >();
			g_MeasureFactory.register_class< EffortMeasure >();
			g_MeasureFactory.register_class< DofLimitMeasure >();
			g_MeasureFactory.register_class< DofMeasure >();
			g_MeasureFactory.register_class< BodyMeasure >();
			g_MeasureFactory.register_class< CompositeMeasure >();
			g_MeasureFactory.register_class< SymmetricMeasure >();
			g_MeasureFactory.register_class< JumpMeasure >();
			g_MeasureFactory.register_class< JointLoadMeasure >();
			g_MeasureFactory.register_class< ReactionForceMeasure >();
			g_MeasureFactory.register_class< PointMeasure >();
			g_MeasureFactory.register_class< BalanceMeasure >();
			g_MeasureFactory.register_class< MimicMeasure >();
		}
		return g_MeasureFactory( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API ReflexUP CreateReflex( const PropNode& props, Params& par, Model& model, const Location& target_area )
	{
		static xo::factory< Reflex, const PropNode&, Params&, Model&, const Location& > g_ReflexFactory;
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
		static xo::factory< Function, const PropNode&, Params& > g_FunctionFactory;
		if ( g_FunctionFactory.empty() )
		{
			g_FunctionFactory.register_class< PieceWiseConstantFunction >();
			g_FunctionFactory.register_class< PieceWiseConstantFunction >( "PieceWiseConstant" );
			g_FunctionFactory.register_class< PieceWiseLinearFunction >();
			g_FunctionFactory.register_class< PieceWiseLinearFunction >( "PieceWiseLinear" );
			g_FunctionFactory.register_class< Polynomial >();
		}
		return g_FunctionFactory( props.get< String >( "type" ), props, par );
	}

	SCONE_API ModelUP CreateModel( const PropNode& prop, Params& par )
	{
		static xo::factory< Model, const PropNode&, Params& > g_ModelFactory;
		if ( g_ModelFactory.empty() )
		{
			g_ModelFactory.register_class< ModelSimbody >( "Simbody" );
		}
		return g_ModelFactory( prop.get< String >( "type" ), prop, par );
	}

	SCONE_API SensorUP CreateSensor( const PropNode& props, Params& par, Model& m, const Location& a )
	{
		static xo::factory< Sensor, const PropNode&, Params&, Model&, const Location& > g_SensorFactory;
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

	SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop )
	{
		static xo::factory< Optimizer, const PropNode& > g_OptimizerFactory;
		if ( g_OptimizerFactory.empty() )
		{
			g_OptimizerFactory.register_class< CmaOptimizerSpot >( "CmaOptimizer" );
			g_OptimizerFactory.register_class< CmaOptimizerCCMAES >();
			g_OptimizerFactory.register_class< CmaOptimizerSpot >();
			g_OptimizerFactory.register_class< CmaPoolOptimizer >();
		}
		return g_OptimizerFactory( prop.get< String >( "type" ), prop );
	}

	SCONE_API xo::factory< Objective, const PropNode& >& GetObjectiveFactory()
	{
		static xo::factory< Objective, const PropNode& > g_ObjectiveFactory;
		if ( g_ObjectiveFactory.empty() )
		{
			g_ObjectiveFactory.register_class< SimulationObjective >();
			g_ObjectiveFactory.register_class< ImitationObjective >();
			g_ObjectiveFactory.register_class< SimilarityObjective >();
			g_ObjectiveFactory.register_class< TestObjective >();
		}
		return g_ObjectiveFactory;
	}

	SCONE_API ObjectiveUP CreateObjective( const PropNode& prop )
	{
		return GetObjectiveFactory()( prop.get< String >( "type" ), prop );
	}
}
