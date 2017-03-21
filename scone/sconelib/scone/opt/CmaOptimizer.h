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

			virtual void Run() override { SCONE_THROW( "Please use a subclass of CmaOptimzer" ); }

		protected:
			int m_Mu;
			int m_Lambda;
			double m_Sigma;
			size_t max_generations;
			int recombination_type; // 0 = equal, 1 = linear, 2 = superlinear

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
