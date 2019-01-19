/*
** Factories.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Factories.h"

#include "scone/controllers/CompositeController.h"
#include "scone/controllers/ConditionalMuscleReflex.h"
#include "scone/controllers/DofReflex.h"
#include "scone/controllers/FeedForwardController.h"
#include "scone/controllers/GaitStateController.h"
#include "scone/controllers/MirrorController.h"
#include "scone/controllers/MuscleReflex.h"
#include "scone/controllers/NeuralController.h"
#include "scone/controllers/NoiseController.h"
#include "scone/controllers/PerturbationController.h"
#include "scone/controllers/ReflexController.h"
#include "scone/controllers/SensorStateController.h"
#include "scone/controllers/SequentialController.h"
#include "scone/controllers/TimeStateController.h"

#include "scone/core/PieceWiseConstantFunction.h"
#include "scone/core/PieceWiseLinearFunction.h"
#include "scone/core/Polynomial.h"

#include "scone/measures/BalanceMeasure.h"
#include "scone/measures/BodyMeasure.h"
#include "scone/measures/CompositeMeasure.h"
#include "scone/measures/DofMeasure.h"
#include "scone/measures/GaitCycleMeasure.h"
#include "scone/measures/GaitMeasure.h"
#include "scone/measures/HeightMeasure.h"
#include "scone/measures/JointLoadMeasure.h"
#include "scone/measures/JumpMeasure.h"
#include "scone/measures/MimicMeasure.h"
#include "scone/measures/ReactionForceMeasure.h"

#include "scone/model/Sensors.h"

#include "scone/optimization/CmaOptimizerSpot.h"
#include "scone/optimization/CmaPoolOptimizer.h"
#include "scone/optimization/ImitationObjective.h"
#include "scone/optimization/SimilarityObjective.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/optimization/TestObjective.h"
#include "../controllers/BodyPointReflex.h"

namespace scone
{
	SCONE_API ControllerUP CreateController( const PropNode& props, Params& par, Model& model, const Location& target_area )
	{
		static xo::factory< Controller, const PropNode&, Params&, Model&, const Location& > g_ControllerFactory;
		if ( g_ControllerFactory.empty() )
		{
			// register controllers
			g_ControllerFactory.register_type< FeedForwardController >();
			g_ControllerFactory.register_type< GaitStateController >();
			g_ControllerFactory.register_type< ReflexController >();
			g_ControllerFactory.register_type< TimeStateController >();
			g_ControllerFactory.register_type< PerturbationController >();
			g_ControllerFactory.register_type< SensorStateController >();
			g_ControllerFactory.register_type< MirrorController >();
			g_ControllerFactory.register_type< NeuralController >();
			g_ControllerFactory.register_type< CompositeController >();
			g_ControllerFactory.register_type< SequentialController >();
			g_ControllerFactory.register_type< NoiseController >();
		}
		return g_ControllerFactory.create( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API MeasureUP CreateMeasure( const PropNode& props, Params& par, Model& model, const Location& target_area )
	{
		static xo::factory< Measure, const PropNode&, Params&, Model&, const Location& > g_MeasureFactory;
		if ( g_MeasureFactory.empty() )
		{
			// register measures
			g_MeasureFactory.register_type< HeightMeasure >();
			g_MeasureFactory.register_type< GaitMeasure >();
			g_MeasureFactory.register_type< GaitCycleMeasure >();
			g_MeasureFactory.register_type< EffortMeasure >();
			g_MeasureFactory.register_type< DofLimitMeasure >();
			g_MeasureFactory.register_type< DofMeasure >();
			g_MeasureFactory.register_type< BodyMeasure >();
			g_MeasureFactory.register_type< CompositeMeasure >();
			g_MeasureFactory.register_type< JumpMeasure >();
			g_MeasureFactory.register_type< JointLoadMeasure >();
			g_MeasureFactory.register_type< ReactionForceMeasure >();
			g_MeasureFactory.register_type< BalanceMeasure >();
			g_MeasureFactory.register_type< MimicMeasure >();
		}
		return g_MeasureFactory.create( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API ReflexUP CreateReflex( const PropNode& props, Params& par, Model& model, const Location& target_area )
	{
		static xo::factory< Reflex, const PropNode&, Params&, Model&, const Location& > g_ReflexFactory;
		if ( g_ReflexFactory.empty() )
		{
			g_ReflexFactory.register_type< MuscleReflex >();
			g_ReflexFactory.register_type< DofReflex >();
			g_ReflexFactory.register_type< BodyPointReflex >();
			g_ReflexFactory.register_type< ConditionalMuscleReflex >();
		}
		return g_ReflexFactory.create( props.get< String >( "type" ), props, par, model, target_area );
	}

	SCONE_API FunctionUP CreateFunction( const PropNode& props, Params& par )
	{
		static xo::factory< Function, const PropNode&, Params& > g_FunctionFactory;
		if ( g_FunctionFactory.empty() )
		{
			g_FunctionFactory.register_type< PieceWiseConstantFunction >();
			g_FunctionFactory.register_type< PieceWiseConstantFunction >( "PieceWiseConstant" );
			g_FunctionFactory.register_type< PieceWiseLinearFunction >();
			g_FunctionFactory.register_type< PieceWiseLinearFunction >( "PieceWiseLinear" );
			g_FunctionFactory.register_type< Polynomial >();
		}
		return g_FunctionFactory.create( props.get< String >( "type" ), props, par );
	}

	SCONE_API OptimizerUP CreateOptimizer( const PropNode& prop )
	{
		static xo::factory< Optimizer, const PropNode& > g_OptimizerFactory;
		if ( g_OptimizerFactory.empty() )
		{
			g_OptimizerFactory.register_type< CmaOptimizerSpot >( "CmaOptimizer" );
			g_OptimizerFactory.register_type< CmaOptimizerSpot >();
			g_OptimizerFactory.register_type< CmaPoolOptimizer >();
		}
		return g_OptimizerFactory.create( prop.get< String >( "type" ), prop );
	}

	SCONE_API ModelFactory& GetModelFactory()
	{
		static ModelFactory g_ModelFactory;
		if ( g_ModelFactory.empty() )
		{
			// all models are registered externally
		}
		return g_ModelFactory;
	}

	SCONE_API ModelUP CreateModel( const PropNode& prop, Params& par )
	{
		return GetModelFactory().create( prop.get< String >( "type" ), prop, par );
	}

	SCONE_API ObjectiveFactory& GetObjectiveFactory()
	{
		static ObjectiveFactory g_ObjectiveFactory;
		if ( g_ObjectiveFactory.empty() )
		{
			g_ObjectiveFactory.register_type< SimulationObjective >();
			g_ObjectiveFactory.register_type< ImitationObjective >();
			g_ObjectiveFactory.register_type< SimilarityObjective >();
			g_ObjectiveFactory.register_type< TestObjective >();
		}
		return g_ObjectiveFactory;
	}

	SCONE_API ObjectiveUP CreateObjective( const PropNode& prop )
	{
		return GetObjectiveFactory().create( prop.get< String >( "type" ), prop );
	}
}
