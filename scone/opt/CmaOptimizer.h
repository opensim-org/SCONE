#pragma once

#include "opt.h"
#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		class OPT_API OptimizerCma : public Optimizer
		{
		public:
			OptimizerCma();
			virtual ~OptimizerCma();

			virtual void ProcessProperties( const PropNode& props ) override;

		private:
			double mu;
			double lambda;
			double sigma;
			int max_generations;
		};
	}
}
