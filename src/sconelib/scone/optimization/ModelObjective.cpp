#include "ModelObjective.h"
#include "scone/core/Factories.h"

namespace scone
{
	ModelObjective::ModelObjective( const PropNode& props ) :
	Objective( props ),
	m_ModelPropsCopy( props.get_child( "Model" ) )
	{
	}

	scone::fitness_t ModelObjective::evaluate( const ParamInstance& point ) const
	{
		auto model = CreateModelFromParams( ParamInstance( point ) );
		return EvaluateModel( *model );
	}

	scone::fitness_t ModelObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceModel( m, GetDuration() );
		return GetResult( m );
	}

	scone::ModelUP ModelObjective::CreateModelFromParams( Params& par ) const
	{
		return CreateModel( m_ModelPropsCopy, par );
	}

	scone::ModelUP ModelObjective::CreateModelFromParFile( const path& parfile ) const
	{
		return CreateModelFromParams( ParamInstance( info_, parfile ) );
	}
}
