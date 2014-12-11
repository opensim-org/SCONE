#pragma once

#include "opt.h"
#include "Objective.h"
#include "..\core\Propertyable.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Optimizer : public Propertyable
		{
		public:
			Optimizer();
			virtual ~Optimizer();

			virtual void ProcessProperties( const PropNode& props ) override;
			virtual void Run( ObjectiveSP objective ) = 0;

		protected:
			std::vector< double > EvaluateSingleThreaded( std::vector< ParamSet >& parsets, ObjectiveSP objective );
			std::vector< double > EvaluateMultiThreaded( std::vector< ParamSet >& parsets, std::vector< ObjectiveSP >& objectives );

		private:
			static void Optimizer::EvaluateFunc( ObjectiveSP obj, ParamSet& par, double* fitness );

			size_t max_threads;
		};
	}
}
