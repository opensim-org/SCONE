#pragma once

#include "CmaOptimizer.h"
#include "spot/cma_optimizer.h"
#include "spot/reporter.h"
#include "xo/system/log_sink.h"

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
		virtual void SetOutputMode( OutputMode m ) override;
		virtual ~CmaOptimizerSpot() {}
		virtual void Run() override;

		xo::log::file_sink log_sink_;
	};

	class SCONE_API CmaOptimizerReporter : public spot::reporter
	{
	public:
		virtual void on_start( const optimizer& opt ) override;
		virtual void on_stop( const optimizer& opt, const spot::stop_condition& s ) override;
		virtual void on_pre_evaluate_population( const optimizer& opt, const search_point_vec& pop ) override;
		virtual void on_post_evaluate_population( const optimizer& opt, const search_point_vec& pop, const fitness_vec_t& fitnesses, index_t best_idx, bool new_best ) override;
	};
}
