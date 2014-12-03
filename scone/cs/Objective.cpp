#include "stdafx.h"
#include "Objective.h"
#include "..\core\Exception.h"

namespace scone
{
	namespace cs
	{
		Objective::Objective()
		{
		}

		Objective::~Objective()
		{
		}

		opt::ParamSet Objective::GetParamSet()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		double Objective::Evaluate( const opt::ParamSet& params )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
