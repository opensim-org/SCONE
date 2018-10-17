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
#include "scone/model/Sensors.h"

namespace scone
{
	MuscleReflex::MuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Reflex( props, par, model, loc ),
	m_pForceSensor( nullptr ),
	m_pLengthSensor( nullptr ),
	m_pVelocitySensor( nullptr ),
	m_pSpindleSensor( nullptr ),
	m_pActivationSensor( nullptr )
	{
		INIT_PROP( props, source, target );

		Muscle& src_mus = *FindByLocation( model.GetMuscles(), source, loc );

		// init names
		String par_name = GetParName( props );
		ScopedParamSetPrefixer prefixer( par, par_name + "." );

		INIT_PAR( props, par, KL, 0.0 );
		INIT_PAR( props, par, L0, 1.0 );
		INIT_PROP( props, allow_neg_L, true );

		INIT_PAR( props, par, KV, 0.0 );
		INIT_PAR( props, par, V0, 0.0 );
		INIT_PROP( props, allow_neg_V, false );

		INIT_PAR( props, par, KF, 0.0 );
		INIT_PAR( props, par, F0, 0.0 );
		INIT_PROP( props, allow_neg_F, true );

		INIT_PAR( props, par, KS, 0.0 );
		INIT_PAR( props, par, S0, 0.0 );
		INIT_PROP( props, allow_neg_S, false );

		INIT_PAR( props, par, KA, 0.0 );
		INIT_PAR( props, par, A0, 0.0 );
		INIT_PROP( props, allow_neg_A, false );

		INIT_PAR( props, par, C0, 0.0 );

		// create delayed sensors
		if ( KF != 0.0 )
			m_pForceSensor = &model.AcquireDelayedSensor< MuscleForceSensor >( src_mus );

		if ( KL != 0.0 )
			m_pLengthSensor = &model.AcquireDelayedSensor< MuscleLengthSensor >( src_mus );

		if ( KV != 0.0 )
			m_pVelocitySensor = &model.AcquireDelayedSensor< MuscleVelocitySensor >( src_mus );

		if ( KS!= 0.0 )
			m_pSpindleSensor = &model.AcquireDelayedSensor< MuscleSpindleSensor >( src_mus );

		if ( KA != 0.0 )
			m_pActivationSensor = &model.AcquireDelayedSensor< MuscleActivationSensor >( src_mus );

		//log::TraceF( "MuscleReflex SRC=%s TRG=%s KL=%.2f KF=%.2f C0=%.2f", source.GetName().c_str(), m_Target.GetName().c_str(), length_gain, force_gain, u_constant );
	}

	MuscleReflex::~MuscleReflex()
	{
	}

	void MuscleReflex::ComputeControls( double timestamp )
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
		AddTargetControlValue( u_total );
	}

	void MuscleReflex::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		auto name = GetReflexName( target, source );
		if ( m_pLengthSensor )
			frame[ name + ".RL" ] = u_l;
		if ( m_pVelocitySensor )
			frame[ name + ".RV" ] = u_v;
		if ( m_pForceSensor )
			frame[ name + ".RF" ] = u_f;
		if ( m_pSpindleSensor )
			frame[ name + ".RS" ] = u_s;
		if ( m_pActivationSensor )
			frame[ name + ".RA" ] = u_a;
		frame[ name + ".R" ] = u_total;
	}
}
