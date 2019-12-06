/*
** Leg.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Leg.h"
#include "Body.h"
#include "Joint.h"
#include "scone/core/profiler_config.h"
#include "scone/core/string_tools.h"
#include "Model.h"
#include "model_tools.h"
#include "scone/core/Log.h"
#include "xo/utility/memory_tools.h"

namespace scone
{
	Leg::Leg( Body& proximal, Body& foot, size_t index, Side side, size_t rank, const ContactForce* cf ) :
		m_Side( side ),
		m_Rank( rank ),
		m_Index( index ),
		m_Upper( proximal ),
		m_Foot( foot ),
		m_Base( xo::dereference_or_throw( proximal.GetParentBody(), proximal.GetName() + "is an upper leg body without a parent" ) ),
		m_Name( stringf( "leg%d", index ) + ( ( side == LeftSide ) ? "_l" : "_r" ) ),
		m_ContactForce( cf )
	{
		// measure length during construction, as it could be pose-dependent
		m_LegLength = MeasureLength();
	}

	Leg::~Leg()
	{
	}

	Vec3 Leg::GetContactForce() const
	{
		return m_ContactForce ? m_ContactForce->GetForce() : m_Foot.GetContactForce();
	}

	Vec3 Leg::GetRelFootPos() const
	{
		return GetFootBody().GetComPos() - GetBaseBody().GetComPos();
	}

	void Leg::GetContactForceMomentCop( Vec3& force, Vec3& moment, Vec3& cop ) const
	{
		std::tie( force, moment, cop ) = m_ContactForce->GetForceMomentPoint();
	}

	Real Leg::MeasureLength() const
	{
		// HACK: this uses body positions because we don't have access to joint positions in OpenSim
		// OpenSim: how can we get the actual position of a joint
		// add all distances from foot to upper, using body origins
		double d = 0.0;
		for ( const Body* body = &m_Foot; body && body->GetParentBody() && body != &m_Upper; body = body->GetParentBody() )
			d += length( body->GetOriginPos() - body->GetParentBody()->GetOriginPos() );

		return d;
	}

	Real Leg::GetLoad() const
	{
		return xo::length( GetContactForce() ) / m_Upper.GetModel().GetBW();
	}
}
