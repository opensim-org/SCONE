#pragma once

#include "opt.h"
#include "ParamSet.h"
#include "../core/Propertyable.h"

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
			ParamSet GetParamSet();

			// update all parameters and call Evaluate
			double Evaluate( ParamSet& par );
			virtual void WriteResults( const String& file ) { };

		protected:
			// virtual evaluation function
			virtual double Evaluate() = 0;
		};
	}
}
