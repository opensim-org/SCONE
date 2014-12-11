#pragma once

#include "opt.h"
#include "ParamSet.h"
#include "..\core\Propertyable.h"
#include "..\core\Exception.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Objective : public Propertyable, public Parameterizable
		{
		public:
			Objective();
			virtual ~Objective();

			// Find all parameters in this objective
			ParamSet GetParSet();

			// update all parameters and call Evaluate
			virtual double Evaluate( ParamSet& par );

			// virtual evaluation function
			virtual double Evaluate() = 0;
		};
	}
}
