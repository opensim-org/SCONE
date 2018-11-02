/*
** DofSimbody.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "DofSimbody.h"
#include "ModelSimbody.h"
#include "JointSimbody.h"
#include "scone/core/Log.h"

#include <OpenSim/OpenSim.h>

namespace scone
{
	DofSimbody::DofSimbody( ModelSimbody& model, OpenSim::Coordinate& coord ) :
	Dof( *FindByName( model.GetJoints(), coord.getJoint().getName() ) ),
	m_Model( model ),
	m_osCoord( coord ),
	m_pOsLimitForce( nullptr )
	{
		// find corresponding CoordinateLimitForce
		auto& forceSet = model.GetOsimModel().getForceSet();
		for ( int idx = 0; idx < forceSet.getSize(); ++idx )
		{
			// OpenSim: Set<T>::get( idx ) is const but returns non-const reference, is this a bug?
			const OpenSim::CoordinateLimitForce* clf = dynamic_cast<const OpenSim::CoordinateLimitForce*>( &forceSet.get( idx ) );
			if ( clf && clf->getProperty_coordinate().getValue() == coord.getName() )
			{
				// we have found a match!
				//log::Trace( "Found limit force " + clf->getName() + " for coord " + coord.getName() );
				m_pOsLimitForce = clf;
				break;
			}
		}
	}

	DofSimbody::~DofSimbody() {}

	scone::Real DofSimbody::GetPos() const
	{
		return m_osCoord.getValue( m_Model.GetTkState() );
	}

	scone::Real DofSimbody::GetVel() const
	{
		return m_osCoord.getSpeedValue( m_Model.GetTkState() );
	}

	const String& DofSimbody::GetName() const
	{
		return m_osCoord.getName();
	}

	scone::Real DofSimbody::GetLimitForce() const
	{
		if ( m_pOsLimitForce )
			return m_pOsLimitForce->calcLimitForce( m_Model.GetTkState() );
		else return 0.0;
	}

	scone::Real DofSimbody::GetMoment() const
	{
		SCONE_THROW_NOT_IMPLEMENTED;
	}

	void DofSimbody::SetPos( Real pos, bool enforce_constraints )
	{
		if ( !m_osCoord.getLocked( m_Model.GetTkState() ) )
			m_osCoord.setValue( m_Model.GetTkState(), pos, enforce_constraints );
	}

	void DofSimbody::SetVel( Real vel )
	{
		if ( !m_osCoord.getLocked( m_Model.GetTkState() ) )
			m_osCoord.setSpeedValue( m_Model.GetTkState(), vel );
	}

	Vec3 DofSimbody::GetRotationAxis() const
	{
		return m_RotationAxis;
	}

	Range< Real > DofSimbody::GetRange() const
	{
		return Range< Real >( m_osCoord.get_range( 0 ), m_osCoord.get_range( 1 ) );
	}
}
