#pragma once

#include "opt.h"
#include "ParamSet.h"
#include "scone/core/InitFromPropNode.h"
#include "scone/core/HasSignature.h"

namespace scone
{
	namespace opt
	{
		class SCONE_API Objective : public HasSignature
		{
		public:
			Objective( const PropNode& props, ParamSet& par );
			virtual ~Objective();

			// get all parameters in this objective
			ParamSet MakeParamSet();

			// update all parameters and call Evaluate
			double Evaluate( ParamSet& par );

			// virtual evaluation function
			virtual double Evaluate() = 0;

			// write results and return all files written
			virtual std::vector< String > WriteResults( const String& file_base ) { return std::vector< String >(); }

			int debug_idx;

		protected:
			// process parameters
			virtual void ProcessParameters( opt::ParamSet& par ) = 0;

		};
	}
}
