/*
** CmaOptimizerSpot.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "CmaOptimizer.h"
#include "spot/cma_optimizer.h"
#include "spot/reporter.h"
#include "spot/file_reporter.h"
#include "xo/system/log_sink.h"
#include "xo/time/timer.h"

namespace scone
{
	using spot::optimizer;
	using spot::search_point;
	using spot::search_point_vec;
	using spot::fitness_vec_t;
	using xo::index_t;

	/// Optimizer based on the CMA-ES algorithm by [Hansen].
	class SCONE_API CmaOptimizerSpot : public CmaOptimizer, public spot::cma_optimizer
	{
	public:
		CmaOptimizerSpot( const PropNode& pn, const PropNode& scenario_pn, const path& scenario_dir );
		virtual void SetOutputMode( OutputMode m ) override;
		virtual ~CmaOptimizerSpot() {}
		virtual void Run() override;
		virtual double GetBestFitness() const override { return best_fitness(); }
	};

	class SCONE_API CmaOptimizerReporter : public spot::reporter
	{
	public:
		virtual void on_start( const optimizer& opt ) override;
		virtual void on_stop( const optimizer& opt, const spot::stop_condition& s ) override;
		virtual void on_pre_evaluate_population( const optimizer& opt, const search_point_vec& pop ) override;
		virtual void on_post_evaluate_population( const optimizer& opt, const search_point_vec& pop, const fitness_vec_t& fitnesses, bool new_best ) override;
		xo::timer timer_;
		size_t number_of_evaluations_;
	};
}
