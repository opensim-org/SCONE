#include "Objective.h"
#include "scone/core/Exception.h"

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

		scone::opt::ParamSet Objective::MakeParamSet()
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
