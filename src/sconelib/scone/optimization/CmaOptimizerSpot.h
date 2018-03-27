#pragma once

#include "CmaOptimizer.h"
#include "spot/cma_optimizer.h"
#include "spot/reporter.h"

namespace scone
{
	using spot::optimizer;
	using spot::search_point;
	using spot::search_point_vec;
	using spot::fitness_vec_t;
	using xo::index_t;

	class SCONE_API CmaOptimizerSpot : public CmaOptimizer, public spot::cma_optimizer
	{
	public:
		CmaOptimizerSpot( const PropNode& pn );
		virtual ~CmaOptimizerSpot() {}
		virtual void Run() override;
	};

	class SCONE_API CmaOptimizerReporter : public spot::reporter
	{
	public:
		virtual void start( const optimizer& opt ) override;
		virtual void finish( const optimizer& opt ) override;
		virtual void evaluate_population_start( const optimizer& opt, const search_point_vec& pop ) override;
		virtual void evaluate_population_finish( const optimizer& opt, const search_point_vec& pop, const fitness_vec_t& fitnesses, index_t best_idx, bool new_best ) override;
	};
}
