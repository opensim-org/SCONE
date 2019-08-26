/*
** Leg.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Leg.h"
#include "Link.h"
#include "Body.h"
#include "Joint.h"
#include "scone/core/profiler_config.h"
#include "scone/core/string_tools.h"
#include "Model.h"
#include "model_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	Leg::Leg( Link& proximal, Link& foot, size_t index, Side side, size_t rank, const ContactForce* cf ) :
		m_Upper( proximal ),
		m_Foot( foot ),
		m_Side( side ),
		m_Index( index ),
		m_Rank( rank ),
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
		return m_ContactForce->GetForce();
	}

	Vec3 Leg::GetContactCop() const
	{
		return m_ContactForce->GetPoint();
	}

	Vec3 Leg::GetRelFootPos() const
	{
		return GetFootLink().GetBody().GetComPos() - GetBaseLink().GetBody().GetComPos();
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
		for ( const Link* body = &m_Foot; body != &m_Upper; body = &body->GetParent() )
			d += length( body->GetBody().GetOriginPos() - body->GetParent().GetBody().GetOriginPos() );

		return d;
	}

	Real Leg::GetLoad() const
	{
		return xo::length( GetContactForce() ) / GetModel().GetBW();
	}

	Model& Leg::GetModel() const
	{
		return m_Upper.GetBody().GetModel();
	}
}
