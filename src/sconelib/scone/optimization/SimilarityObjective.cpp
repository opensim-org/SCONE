/*
** SimilarityObjective.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SimilarityObjective.h"
#include "scone/core/Factories.h"
#include "xo/container/prop_node_tools.h"
#include "spot/search_point.h"
#include "scone/controllers/NeuralController.h"
#include "scone/core/profiler_config.h"
#include "opt_tools.h"

namespace scone
{
	SimilarityObjective::SimilarityObjective( const PropNode& pn ) :
	ModelObjective( pn ),
	result_( 0 )
	{
		// #issue84: this model should be kept, but not used by Optimizer
		auto model = InitializeModelObjective( pn );

		INIT_PROP_REQUIRED( pn, file_ );
		AddExternalResource( file_ );

		// load target model
		// scenario_pn can be local because the model is created in the same scope
		auto scenario_pn = xo::load_file_with_include( FindScenario( file_ ), "INCLUDE" );
		target_ = CreateModelObjective( scenario_pn, file_.parent_path() );
		target_model_ = target_->CreateModelFromParFile( file_ );
	}

	void SimilarityObjective::AdvanceSimulationTo( Model& m, TimeInSeconds t ) const
	{
		EvaluateModel( m );
	}

	fitness_t SimilarityObjective::GetResult( Model& m ) const
	{
		return result_;
	}

	PropNode SimilarityObjective::GetReport( Model& m ) const
	{
		return xo::to_prop_node( GetResult( m ) );
	}

	fitness_t SimilarityObjective::EvaluateModel( Model& m ) const
	{
		SCONE_PROFILE_FUNCTION;

		auto& c1 = dynamic_cast<const NeuralController&>( *target_model_->GetController() );
		auto& c2 = dynamic_cast<const NeuralController&>( *m.GetController() );
		result_ = c1.GetSimilarity( c2 );
		return result_;
	}

	String SimilarityObjective::GetClassSignature() const
	{
		return signature_;
	}
}
