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

			SCONE_GENERATE_FACTORY_MEMBERS( CmaOptimizer );

			virtual void ProcessProperties( const PropNode& props ) override;
			virtual void Run( ObjectiveSP m_Objective ) override;

		private:
			ObjectiveSP m_Objective;

			double mu;
			double lambda;
			double sigma;
			int max_generations;
		};
	}
}
