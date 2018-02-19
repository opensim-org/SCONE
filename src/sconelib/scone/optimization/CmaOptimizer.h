#pragma once

#include "Optimizer.h"

namespace scone
{
	class SCONE_API CmaOptimizer : public Optimizer
	{
	public:
		CmaOptimizer( const PropNode& props );
		CmaOptimizer( const CmaOptimizer& ) = delete;
		CmaOptimizer& operator=( const CmaOptimizer& ) = delete;
		virtual ~CmaOptimizer();

		virtual void Run() override { SCONE_THROW( "Please use a subclass of CmaOptimzer" ); }

	protected:
		int mu_;
		int lambda_;
		double sigma_;
		size_t max_generations;
		size_t window_size;
		int recombination_type; // 0 = equal, 1 = linear, 2 = superlinear

		double global_std_factor;
		double global_std_offset;

		bool use_init_file_std;
		double init_file_std_factor;
		double init_file_std_offset;

		size_t num_elitists;
		int max_attempts;
		long random_seed;

	private: // non-copyable and non-assignable
		virtual String GetClassSignature() const override;
	};
}
