/*
** BodyPointReflex.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "BodyPointReflex.h"
#include "scone/model/Sensors.h"
#include "scone/model/Model.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Actuator.h"
#include "scone/core/HasName.h"

namespace scone
{
	BodyPointReflex::BodyPointReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
		Reflex( props, par, model, loc ),
		INIT_MEMBER_REQUIRED( props, source ),
		INIT_MEMBER( props, offset, Vec3::zero() ),
		INIT_MEMBER( props, direction, Vec3::zero() ),
		body_( *FindByNameTrySided( model.GetBodies(), source, loc.side_ ) ),
		m_DelayedPos( model.AcquireDelayedSensor< BodyPointPositionSensor >( body_, offset, direction ) ),
		m_DelayedVel( model.AcquireDelayedSensor< BodyPointVelocitySensor >( body_, offset, direction ) ),
		m_DelayedAcc( model.AcquireDelayedSensor< BodyPointAccelerationSensor >( body_, offset, direction ) )
	{
		ScopedParamSetPrefixer prefixer( par, GetParName( props, loc ) + "." );

		INIT_PAR_NAMED( props, par, P0, "P0", 0.0 );
		INIT_PAR_NAMED( props, par, KP, "KP", 0.0 );

		INIT_PAR_NAMED( props, par, V0, "V0", 0.0 );
		INIT_PAR_NAMED( props, par, KV, "KV", 0.0 );

		INIT_PAR_NAMED( props, par, A0, "A0", 0.0 );
		INIT_PAR_NAMED( props, par, KA, "KA", 0.0 );

		INIT_PAR_NAMED( props, par, C0, "C0", 0.0 );
	}

	void BodyPointReflex::ComputeControls( double timestamp )
	{
		Real pos = m_DelayedPos.GetValue( delay );
		Real vel = m_DelayedVel.GetValue( delay );
		Real acc = m_DelayedAcc.GetValue( delay );
		u_p = KP * ( P0 - pos );
		u_v = KV * ( V0 - vel );
		u_a = KA * ( A0 - acc );
		AddTargetControlValue( C0 + u_p + u_v + u_a );
	}

	void BodyPointReflex::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		auto name = GetReflexName( actuator_.GetName(), source );
		frame[ name + ".RBP" ] = u_p;
		frame[ name + ".RBV" ] = u_v;
		frame[ name + ".RBA" ] = u_a;
	}
}
