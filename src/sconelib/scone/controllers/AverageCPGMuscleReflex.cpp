/*
** AverageCPGMuscleReflex.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "AverageCPGMuscleReflex.h"
#include "scone/model/Muscle.h"

namespace scone
{
	AverageCPGMuscleReflex::AverageCPGMuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	MuscleReflex( props, par, model, loc )
	{
		INIT_PAR( props, par, alpha, 0.0 );
	}

	AverageCPGMuscleReflex::~AverageCPGMuscleReflex()
	{
	}

	void AverageCPGMuscleReflex::ComputeControls( double timestamp )
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

		// append CPG
		u_cpg.append(u_total);

		AddTargetControlValue( alpha * u_cpg.average() + (1 - alpha) * u_total );
	}

	void AverageCPGMuscleReflex::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		MuscleReflex::StoreData(frame, flags);
		auto name = GetReflexName( actuator_.GetName(), source.GetName() );

		frame[ name + ".cpg_avg" ] = u_cpg.average();
	}
}
