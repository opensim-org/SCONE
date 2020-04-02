/*
** AverageCPGMuscleReflex.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "MuscleReflex.h"

namespace scone
{
	/// A reflex that combines feed-forward control from a central pattern
	/// generator (CPG) and muscle reflex (all functionalities from
	/// MuscleReflex)..
	///
	/// Must be part of ReflexController. Output excitation corresponds to
	///
	/// ''U = alpha * U_cpg + (1 - alpha) * U_reflex'',
	////
	/// where a in [0, 1], U_cpg is the average feedback signal. The average
	/// feedback signal is calculated only when the feedback is active.
	class AverageCPGMuscleReflex : public MuscleReflex
	{
	public:
		AverageCPGMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~AverageCPGMuscleReflex();

		virtual void ComputeControls( double timestamp ) override;

		/// Constant reflecting the contribution of CPG input to the output; default = 0.
		Real alpha;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		struct AverageCPG {
			Real sum = 0;
			Real n = 0;
			void append(Real value) { sum += value; n++; }
			Real average() const { return sum / n; }
		} u_cpg;
	};
}
