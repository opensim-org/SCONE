#pragma once

#include "opt.h"
#include "ParamSet.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Objective
		{
		public:
			Objective();
			virtual ~Objective();

			virtual ParamSet GetParamSet() = 0;
			virtual double Evaluate( const ParamSet& params ) = 0;
		};
	}
}
