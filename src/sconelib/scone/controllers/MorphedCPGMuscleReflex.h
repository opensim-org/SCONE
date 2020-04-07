/*
** MorphedCPGMuscleReflex.h
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
	/// MuscleReflex).
	///
	/// Must be part of ReflexController. Output excitation corresponds to
	///
	/// ''U = alpha * U_cpg + (1 - alpha) * U_reflex'',
	////
	/// where a in [0, 1], U_cpg is computed from the state output of
	/// a morphed oscillator.
	class MorphedCPGMuscleReflex : public MuscleReflex
	{
	public:
		MorphedCPGMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc );

		/// Constant reflecting the contribution of CPG input to the
		/// output; default = 0.
		Real alpha;

		/// The output state name of the morphed oscillator. If
		/// oscillator name is CPG and it has two states with the
		/// second being its output, then cpg_output = CPG_1.
		String cpg_output;

		virtual void ComputeControls( double timestamp ) override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Model* m_model;
	};
}
