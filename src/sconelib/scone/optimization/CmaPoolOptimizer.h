#pragma once

#include "Optimizer.h"
#include "spot/optimizer_pool.h"

namespace scone
{
	class CmaPoolOptimizer : public Optimizer, public spot::optimizer_pool
	{
	public:
		CmaPoolOptimizer( const PropNode& pn );
		virtual ~CmaPoolOptimizer() {}

		virtual void Run() override;
		virtual void SetOutputMode( OutputMode m ) override;

	protected:
		std::vector< PropNode > props_;
		size_t optimization_count_;
		long random_seed_;
	};
}
