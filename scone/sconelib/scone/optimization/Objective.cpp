#include "Objective.h"
#include "scone/core/Exception.h"

namespace scone
{
	Objective::Objective( const PropNode& props, ParamSet& par ) :
	HasSignature( props ),
	debug_idx( -1 ),
	minimize_( true )
	{}

	Objective::~Objective()
	{}

	scone::ParamSet Objective::GetParamInfo()
	{
		ParamSet par( ParamSet::ConstructionMode );
		ProcessParameters( par );
		return par;
	}

	double Objective::Evaluate( ParamSet& par )
	{
		par.SetMode( ParamSet::UpdateMode );
		ProcessParameters( par );

		return Evaluate();
	}

	flut::fitness_t Objective::evaluate( const flut::par_vec& point ) const
	{

	}
}
