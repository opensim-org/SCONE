/*
** Factories.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
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
#include "scone/controllers/SequentialController.h"
#include "scone/controllers/BodyPointReflex.h"

#include "scone/core/PieceWiseConstantFunction.h"
#include "scone/core/PieceWiseLinearFunction.h"
#include "scone/core/Polynomial.h"
#include "scone/core/SineWave.h"

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
#include "scone/measures/MuscleMeasure.h"
#include "scone/measures/StepMeasure.h"

#include "scone/optimization/CmaOptimizerSpot.h"
#include "scone/optimization/CmaPoolOptimizer.h"
#include "scone/optimization/ImitationObjective.h"
#include "scone/optimization/SimilarityObjective.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/optimization/TestObjective.h"
#include "scone/controllers/NeuralNetworkController.h"
#include "xo/filesystem/filesystem.h"
#include "xo/string/string_tools.h"

#define SCONE_WRAP_EXCEPTION( _func_, _msg_ ) \
try { _func_; } catch( std::exception& e ) { SCONE_ERROR( _msg_ + std::string( ":\n   " ) + xo::replace_str( e.what(), "\n", "\n   " ) ); }

namespace scone
{
	ControllerFactory& GetControllerFactory()
	{
		static ControllerFactory g_ControllerFactory = ControllerFactory()
#ifdef SCONE_EXPERIMENTAL_FEATURES
			.register_type< NN::NeuralNetworkController >( "NeuralNetworkController" )
#endif
			.register_type< FeedForwardController >()
			.register_type< GaitStateController >()
			.register_type< ReflexController >()
			.register_type< PerturbationController >()
			.register_type< MirrorController >()
			.register_type< NeuralController >()
			.register_type< CompositeController >()
			.register_type< SequentialController >()
			.register_type< NoiseController >();

		return g_ControllerFactory;
	}

	ControllerUP CreateController( const FactoryProps& fp, Params& par, Model& model, const Location& target_area )
	{
		ScopedParamSetPrefixer param_prefix( par, fp.props().get<String>( "name", "" ), true );
		SCONE_WRAP_EXCEPTION(
			return GetControllerFactory().create( fp.type(), fp.props(), par, model, target_area ),
			fp.type() );
	}

	ControllerUP CreateController( const PropNode& pn, Params& par, Model& model, const Location& target_area )
	{
		ScopedParamSetPrefixer param_prefix( par, pn.get<String>( "name", "" ), true );
		return GetControllerFactory().create( pn.get< String >( "type" ), pn, par, model, target_area );
	}

	MeasureFactory& GetMeasureFactory()
	{
		static MeasureFactory g_MeasureFactory = MeasureFactory()
			.register_type< HeightMeasure >()
			.register_type< GaitMeasure >()
			.register_type< GaitCycleMeasure >()
			.register_type< EffortMeasure >()
			.register_type< DofLimitMeasure >()
			.register_type< DofMeasure >()
			.register_type< MuscleMeasure >()
			.register_type< BodyMeasure >()
			.register_type< CompositeMeasure >()
			.register_type< JumpMeasure >()
			.register_type< JointLoadMeasure >()
			.register_type< ReactionForceMeasure >()
			.register_type< BalanceMeasure >()
			.register_type< MimicMeasure >()
			.register_type< StepMeasure >();

 		return g_MeasureFactory;
	}

	MeasureUP CreateMeasure( const FactoryProps& fp, Params& par, const Model& model, const Location& target_area )
	{
		SCONE_WRAP_EXCEPTION(
			return GetMeasureFactory().create( fp.type(), fp.props(), par, model, target_area ),
			fp.type() );
	}

	MeasureUP CreateMeasure( const PropNode& pn, Params& par, const Model& model, const Location& target_area )
	{
		return GetMeasureFactory().create( pn.get< String >( "type" ), pn, par, model, target_area );
	}

	ReflexFactory& GetReflexFactory()
	{
		static ReflexFactory g_ReflexFactory = ReflexFactory()
			.register_type< MuscleReflex >()
			.register_type< DofReflex >()
			.register_type< BodyPointReflex >()
			.register_type< ConditionalMuscleReflex >();

		return g_ReflexFactory;
	}

	ReflexUP CreateReflex( const FactoryProps& fp, Params& par, Model& model, const Location& target_area )
	{
		return GetReflexFactory().create( fp.type(), fp.props(), par, model, target_area );
	}

	FunctionFactory& GetFunctionFactory()
	{
		static FunctionFactory g_FunctionFactory = FunctionFactory()
			.register_type< PieceWiseConstantFunction >()
			.register_type< PieceWiseConstantFunction >( "PieceWiseConstant" )
			.register_type< PieceWiseLinearFunction >()
			.register_type< PieceWiseLinearFunction >( "PieceWiseLinear" )
			.register_type< Polynomial >()
			.register_type< SineWave >();

		return g_FunctionFactory;
	}

	FunctionUP CreateFunction( const FactoryProps& fp, Params& par )
	{
		return GetFunctionFactory().create( fp.type(), fp.props(), par );
	}

	OptimizerFactory& GetOptimizerFactory()
	{
		static OptimizerFactory g_OptimizerFactory = OptimizerFactory()
			.register_type< CmaOptimizerSpot >( "CmaOptimizer" )
			.register_type< CmaOptimizerSpot >()
			.register_type< CmaPoolOptimizer >();

		return g_OptimizerFactory;
	}

	OptimizerUP CreateOptimizer( const PropNode& props, const path& scenario_dir )
	{
		auto fp = FindFactoryProps( GetOptimizerFactory(), props, "Optimizer" );
		return GetOptimizerFactory().create( fp.type(), fp.props(), props, scenario_dir );
	}

	ModelFactory& GetModelFactory()
	{
		static ModelFactory g_ModelFactory;

		// all models are registered from different dlls in scone_config.h

		return g_ModelFactory;
	}

	ModelUP CreateModel( const FactoryProps& fp, Params& par, const path& scenario_dir )
	{
		xo::current_find_file_path( scenario_dir );
		SCONE_WRAP_EXCEPTION(
			return GetModelFactory().create( fp.type(), fp.props(), par ),
			fp.type() );
	}

	StateComponentFactory& GetStateComponentFactory()
	{
		static StateComponentFactory g_StateComponentFactory = StateComponentFactory();
		// TODO register state components

		return g_StateComponentFactory;
	}

	StateComponentUP CreateStateComponent( const FactoryProps& fp, Params& par, Model& model )
	{
		return GetStateComponentFactory().create( fp.type(), fp.props(), par, model );
	}

	ObjectiveFactory& GetObjectiveFactory()
	{
		static ObjectiveFactory g_ObjectiveFactory = ObjectiveFactory()
			.register_type< SimulationObjective >()
			.register_type< ImitationObjective >()
			.register_type< SimilarityObjective >()
			.register_type< TestObjective >();

		return g_ObjectiveFactory;
	}

	ObjectiveUP CreateObjective( const FactoryProps& fp, const path& find_file_folder )
	{
		return GetObjectiveFactory().create( fp.type(), fp.props(), find_file_folder );
	}
}
