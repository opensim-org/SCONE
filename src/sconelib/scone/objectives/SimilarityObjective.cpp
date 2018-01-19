#include "SimilarityObjective.h"
#include "../core/Factories.h"
#include "xo/serialization/prop_node_tools.h"
#include "../controllers/cs_tools.h"
#include "spot/search_point.h"
#include "../controllers/NeuralController.h"
#include "../core/Profiler.h"

namespace scone
{
	SimilarityObjective::SimilarityObjective( const PropNode& pn ) :
	ModelObjective( pn ),
	result_( 0 )
	{
		INIT_PROP_REQUIRED( pn, file_ );

		AddExternalResource( file_ );

		// create model to flag unused model props and create par_info_
		auto model = CreateModel( pn.get_child( "Model" ), info_ );
		signature_ = model->GetSignature();
		AddExternalResources( model->GetExternalResources() );

		// load target model (TODO: this should be one function call?)
		target_ = CreateModelObjective( file_ );
		target_model_ = target_->CreateModelFromParFile( file_ );
	}

	void SimilarityObjective::AdvanceModel( Model& m, TimeInSeconds t ) const
	{
		EvaluateModel( m );
	}

	scone::fitness_t SimilarityObjective::GetResult( Model& m ) const
	{
		return result_;
	}

	PropNode SimilarityObjective::GetReport( Model& m ) const
	{
		return xo::make_prop_node( GetResult( m ) );
	}

	scone::fitness_t SimilarityObjective::EvaluateModel( Model& m ) const
	{
		SCONE_PROFILE_FUNCTION;

		auto& c1 = dynamic_cast<const NeuralController&>( *target_model_->GetControllers().front() );
		auto& c2 = dynamic_cast<const NeuralController&>( *m.GetControllers().front() );
		result_ = c1.GetSimilarity( c2 );
		return result_;
	}

	String SimilarityObjective::GetClassSignature() const
	{
		return signature_;
	}
}
