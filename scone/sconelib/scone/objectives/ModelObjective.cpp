#include "ModelObjective.h"
#include "../core/Factories.h"

namespace scone
{
	ModelObjective::ModelObjective( const PropNode& props ) :
	Objective( props ),
	m_ModelPropsCopy( props.get_child( "Model" ) )
	{
	}

	scone::fitness_t ModelObjective::evaluate( const ParamInstance& point ) const
	{
		auto model = CreateModel( m_ModelPropsCopy, ParamInstance( point ) );
		return EvaluateModel( *model );
	}

	scone::fitness_t ModelObjective::EvaluateModel( Model& m ) const
	{
		m.SetSimulationEndTime( GetDuration() );
		AdvanceModel( m, GetDuration() );
		return GetResult( m );
	}

	scone::ModelUP ModelObjective::CreateModelFromParameters( Params& par ) const
	{
		return CreateModel( m_ModelPropsCopy, par );
	}

	scone::ModelUP ModelObjective::CreateModelFromParFile( const path& parfile ) const
	{
		return CreateModel( m_ModelPropsCopy, ParamInstance( info_, parfile ) );
	}
}
