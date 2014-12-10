#pragma once

#include "opt.h"
#include "ParamSet.h"
#include "..\core\Propertyable.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Objective : public Propertyable, public Parameterizable
		{
		public:
			Objective();
			virtual ~Objective();

			virtual double Evaluate() = 0;
		};
	}
}
