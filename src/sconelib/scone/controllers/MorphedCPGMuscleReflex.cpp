/*
** MorphedCPGMuscleReflex.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MorphedCPGMuscleReflex.h"
#include "scone/model/Muscle.h"

namespace scone
{
	MorphedCPGMuscleReflex::MorphedCPGMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
		MuscleReflex( props, par, model, loc ),
		INIT_MEMBER_REQUIRED( props, cpg_output ),
		m_model( model )
	{
		INIT_PAR( props, par, alpha, 0.0 );
		SCONE_THROW_IF(alpha < 0 || alpha > 1, "alpha parameter should be 0 <= alpha <= 1");
	}

	void MorphedCPGMuscleReflex::ComputeControls( double timestamp )
	{
		// add stretch reflex
		u_l = GetValue( m_pLengthSensor, KL, L0, allow_neg_L );

		// add velocity reflex
		u_v = GetValue( m_pVelocitySensor, KV, V0, allow_neg_V );

		// add force reflex
		u_f = GetValue( m_pForceSensor, KF, F0, allow_neg_F );

		// add spindle reflex
		u_s = GetValue( m_pSpindleSensor, KS, S0, allow_neg_S );

		// add spindle reflex
		u_a = GetValue( m_pActivationSensor, KA, A0, allow_neg_A );

		// sum it up
		u_total = u_l + u_v + u_f + u_s + u_a + C0;

		auto u_cpg = m_model.GetState().GetValue(cpg_output);

		AddTargetControlValue( alpha * u_cpg + (1 - alpha) * u_total );
	}

	void MorphedCPGMuscleReflex::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		MuscleReflex::StoreData(frame, flags);
		auto name = GetReflexName( actuator_.GetName(), source.GetName() );
		frame[ name + ".cpg_morphed" ] = m_model.GetState().GetValue(cpg_output);
	}
}
