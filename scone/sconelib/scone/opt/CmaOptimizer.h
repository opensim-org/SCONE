#pragma once

#include "Optimizer.h"

namespace scone
{
	namespace opt
	{
		class SCONE_API CmaOptimizer : public Optimizer
		{
		public:
			CmaOptimizer( const PropNode& props );
			virtual ~CmaOptimizer();

			virtual void Run() override;

		private:
			int m_Mu;
			int m_Lambda;
			double m_Sigma;
			size_t max_generations;

			double global_std_factor;
			double global_std_offset;

			size_t num_elitists;
			int max_attempts;
			long random_seed;

		private: // non-copyable and non-assignable
			CmaOptimizer( const CmaOptimizer& );
			CmaOptimizer& operator=( const CmaOptimizer& );

			virtual String GetClassSignature() const override;
		};
	}
}
