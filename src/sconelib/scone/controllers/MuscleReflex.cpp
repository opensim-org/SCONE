/*
** MuscleReflex.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "MuscleReflex.h"
#include "scone/model/Muscle.h"
#include "scone/model/Location.h"
#include "scone/model/Dof.h"

namespace scone
{
	MuscleReflex::MuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Reflex( props, par, model, loc ),
	m_pForceSensor( nullptr ),
	m_pLengthSensor( nullptr ),
	m_pVelocitySensor( nullptr ),
	m_pSpindleSensor( nullptr )
	{
		INIT_PROP( props, source, target );

		Muscle& src_mus = *FindByLocation( model.GetMuscles(), source, loc );

		// init names
		String par_name = GetParName( props );
		name = GetReflexName( m_Target.GetName(), src_mus.GetName() );
		ScopedParamSetPrefixer prefixer( par, par_name + "." );

		INIT_PAR_NAMED( props, par, KL, "KL", 0.0 );
		INIT_PAR_NAMED( props, par, K0, "L0", 1.0 );
		INIT_PROP_NAMED( props, allow_neg_L, "allow_neg_L", true );

		INIT_PAR_NAMED( props, par, KV, "KV", 0.0 );
		INIT_PAR_NAMED( props, par, V0, "V0", 0.0 );
		INIT_PROP_NAMED( props, allow_neg_V, "allow_neg_V", false );

		INIT_PAR_NAMED( props, par, KF, "KF", 0.0 );
		INIT_PAR_NAMED( props, par, F0, "F0", 0.0 );
		INIT_PROP_NAMED( props, allow_neg_F, "allow_neg_F", true );

		INIT_PAR_NAMED( props, par, KS, "KS", 0.0 );
		INIT_PAR_NAMED( props, par, S0, "S0", 0.0 );
		INIT_PROP_NAMED( props, allow_neg_S, "allow_neg_S", false );

		INIT_PAR_NAMED( props, par, C0, "C0", 0.0 );

		// create delayed sensors
		if ( KF != 0.0 )
			m_pForceSensor = &model.AcquireDelayedSensor< MuscleForceSensor >( src_mus );

		if ( KL != 0.0 )
			m_pLengthSensor = &model.AcquireDelayedSensor< MuscleLengthSensor >( src_mus );

		if ( KV != 0.0 )
			m_pVelocitySensor = &model.AcquireDelayedSensor< MuscleVelocitySensor >( src_mus );

		if ( KS!= 0.0 )
			m_pSpindleSensor = &model.AcquireDelayedSensor< MuscleSpindleSensor >( src_mus );

		//log::TraceF( "MuscleReflex SRC=%s TRG=%s KL=%.2f KF=%.2f C0=%.2f", source.GetName().c_str(), m_Target.GetName().c_str(), length_gain, force_gain, u_constant );
	}

	MuscleReflex::~MuscleReflex()
	{
	}

	void MuscleReflex::ComputeControls( double timestamp )
	{
		// add stretch reflex
		u_l = m_pLengthSensor ? KL * ( m_pLengthSensor->GetValue( delay ) - K0 ) : 0;
		if ( !allow_neg_L && u_l < 0.0 ) u_l = 0.0;

		// add velocity reflex
		u_v = m_pVelocitySensor ? KV * ( m_pVelocitySensor->GetValue( delay ) - V0 ) : 0;
		if ( !allow_neg_V && u_v < 0.0 ) u_v = 0.0;

		// add force reflex
		u_f = m_pForceSensor ? KF * ( m_pForceSensor->GetValue( delay ) - F0 ) : 0;
		if ( !allow_neg_F && u_f < 0.0 ) u_f = 0.0;

		// add spindle reflex
		u_s = m_pSpindleSensor ? KS * ( m_pSpindleSensor->GetValue( delay ) - S0 ) : 0;
		if ( !allow_neg_S && u_s < 0.0 ) u_s = 0.0;

		// sum it up
		u_total = u_l + u_v + u_f + u_s + C0;
		AddTargetControlValue( u_total );
	}

	void MuscleReflex::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		if ( m_pLengthSensor )
			frame[ name + ".RL" ] = u_l;
		if ( m_pVelocitySensor )
			frame[ name + ".RV" ] = u_v;
		if ( m_pForceSensor )
			frame[ name + ".RF" ] = u_f;
		if ( m_pSpindleSensor )
			frame[ name + ".RS" ] = u_s;
		frame[ name + ".R" ] = u_total;
	}
}
