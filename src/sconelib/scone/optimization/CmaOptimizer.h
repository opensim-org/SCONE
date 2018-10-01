/*
** CmaOptimizer.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Optimizer.h"
#include "scone/core/Exception.h"

namespace scone
{
	/// Optimizer based on the CMA-ES algorithm by [Hansen].
	/// This is an abstract base class, use a derived class instead.
	class SCONE_API CmaOptimizer : public Optimizer
	{
	public:
		CmaOptimizer( const PropNode& props );
		CmaOptimizer( const CmaOptimizer& ) = delete;
		CmaOptimizer& operator=( const CmaOptimizer& ) = delete;
		virtual ~CmaOptimizer();

		virtual void Run() override { SCONE_THROW( "Please use a subclass of CmaOptimzer" ); }

		/// Mu parameter of CMA-ES.
		int mu_;

		/// Lambda parameter of CMA-ES.
		int lambda_;

		/// Initial Sigma parameter of CMA-ES.
		double sigma_;

		/// Window size used for fitness prediction.
		size_t window_size;

		/// Random seed that will determine the randomness during the optimization.
		long random_seed;

		/// Epsilon value to detect flat fitness; default = 1e-6.
		double flat_fitness_epsilon_;

		int max_attempts;

	private: // non-copyable and non-assignable
		virtual String GetClassSignature() const override;
	};
}
