/*
** CmaPoolOptimizer.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Optimizer.h"
#include "spot/optimizer_pool.h"
#include "xo/system/log_sink.h"

namespace scone
{
	/// Multiple CMA-ES optimizations than run in a prioritized fashion, based on their predicted fitness.
	class CmaPoolOptimizer : public Optimizer, public spot::optimizer_pool
	{
	public:
		CmaPoolOptimizer( const PropNode& pn, const PropNode& root );
		virtual ~CmaPoolOptimizer() {}

		virtual void Run() override;
		virtual void SetOutputMode( OutputMode m ) override;

		/// Maximum number of optimizations; default = 3.
		size_t optimizations_;

		/// Random seed of the first optimization; default = 1.
		long random_seed_;

		virtual double GetBestFitness() const override { return best_fitness(); }

	protected:
		std::vector< PropNode > props_;
	};

	class SCONE_API CmaPoolOptimizerReporter : public spot::reporter
	{
	public:
		virtual void on_start( const spot::optimizer& opt ) override;
		virtual void on_stop( const spot::optimizer& opt, const spot::stop_condition& s ) override;
	};
}
