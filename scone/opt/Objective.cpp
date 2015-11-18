#include "stdafx.h"
#include "Objective.h"
#include "../core/Exception.h"

namespace scone
{
	namespace opt
	{
		Objective::Objective( const PropNode& props, ParamSet& par ) :
		HasSignature( props ),
		debug_idx( -1 )
		{

		}

		Objective::~Objective()
		{

		}

		scone::opt::ParamSet Objective::GetParamSet()
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
	}
}
