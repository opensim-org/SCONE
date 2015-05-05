#pragma once

#include "opt.h"
#include "ParamSet.h"
#include "../core/InitFromPropNode.h"
#include "../core/Signature.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Objective : public Signature
		{
		public:
			Objective( const PropNode& props, ParamSet& par );
			virtual ~Objective();

			// get all parameters in this objective
			ParamSet GetParamSet();

			// update all parameters and call Evaluate
			double Evaluate( ParamSet& par );

			// write results and return all files written
			virtual std::vector< String > WriteResults( const String& file_base ) { return std::vector< String >(); }

			// process parameters
			virtual void ProcessParameters( opt::ParamSet& par ) = 0;

			// virtual evaluation function
			virtual double Evaluate() = 0;

			int debug_idx;
		};
	}
}
