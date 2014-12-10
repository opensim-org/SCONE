#pragma once

#include "opt.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		class OPT_API CmaOptimizer : public Optimizer
		{
		public:
			CmaOptimizer();
			virtual ~CmaOptimizer();

			virtual void ProcessProperties( const PropNode& props ) override;

			SCONE_GENERATE_FACTORY_MEMBERS( CmaOptimizer );

		private:
			double mu;
			double lambda;
			double sigma;
			int max_generations;
		};
	}
}
