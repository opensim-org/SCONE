/*
** SimilarityObjective.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SimilarityObjective.h"
#include "scone/core/Factories.h"
#include "xo/container/prop_node_tools.h"
#include "xo/serialization/serialize.h"
#include "spot/search_point.h"
#include "scone/controllers/NeuralController.h"
#include "scone/core/profiler_config.h"
#include "opt_tools.h"

namespace scone
{
	SimilarityObjective::SimilarityObjective( const PropNode& pn, const path& find_file_folder ) :
	ModelObjective( pn, find_file_folder ),
	result_( 0 )
	{
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
		EvaluateModel( m, xo::stop_token() );
	}

	fitness_t SimilarityObjective::GetResult( Model& m ) const
	{
		return result_;
	}

	PropNode SimilarityObjective::GetReport( Model& m ) const
	{
		return xo::to_prop_node( GetResult( m ) );
	}

	result<fitness_t> SimilarityObjective::EvaluateModel( Model& m, const xo::stop_token& st ) const
	{
		SCONE_PROFILE_FUNCTION( m.GetProfiler() );

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
