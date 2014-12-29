#include "stdafx.h"
#include "Objective.h"
#include "..\core\Exception.h"

namespace scone
{
	namespace opt
	{
		Objective::Objective()
		{

		}

		Objective::~Objective()
		{

		}

		scone::opt::ParamSet Objective::GetParamSet()
		{
			ParamSet par( ParamSet::CONSTRUCTION_MODE );
			ProcessParameters( par );
			return par;
		}

		double Objective::Evaluate( ParamSet& par )
		{
			par.SetMode( ParamSet::UPDATE_MODE );
			ProcessParameters( par );

			return Evaluate();
		}
	}
}
