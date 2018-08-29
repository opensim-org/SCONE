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

	class SCONE_API CmaPoolOptimizerReporter : public spot::reporter
	{
	public:
		virtual void on_start( const spot::optimizer& opt ) override;
		virtual void on_stop( const spot::optimizer& opt, const spot::stop_condition& s ) override;
	};
}
