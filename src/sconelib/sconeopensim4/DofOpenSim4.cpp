/*
** DofOpenSim4.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "DofOpenSim4.h"
#include "ModelOpenSim4.h"
#include "JointOpenSim4.h"
#include "scone/core/Log.h"

#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/ForceSet.h>
#include <OpenSim/Simulation/Model/CoordinateLimitForce.h>
#include <OpenSim/Actuators/CoordinateActuator.h>

namespace scone
{
	Joint* try_find_joint( ModelOpenSim4& model, OpenSim::Coordinate& coord ) {
		if ( auto it = TryFindByName( model.GetJoints(), coord.getJoint().getName() ); it != model.GetJoints().end() )
			return it->get();
		else return nullptr;
	}

	DofOpenSim4::DofOpenSim4( ModelOpenSim4& model, OpenSim::Coordinate& coord ) :
		Dof( try_find_joint( model, coord ) ),
		m_Model( model ),
		m_osCoord( coord ),
		m_pOsLimitForce( nullptr ),
		m_OsCoordAct( nullptr )
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
				m_pOsLimitForce = clf;
				break;
			}
		}
	}

	DofOpenSim4::~DofOpenSim4() {}

	Real DofOpenSim4::GetPos() const
	{
		return m_osCoord.getValue( m_Model.GetTkState() );
	}

	Real DofOpenSim4::GetVel() const
	{
		return m_osCoord.getSpeedValue( m_Model.GetTkState() );
	}

	Real DofOpenSim4::GetAcc() const
	{
		return m_osCoord.getAccelerationValue( m_Model.GetTkState() );
	}

	const String& DofOpenSim4::GetName() const
	{
		return m_osCoord.getName();
	}

	Real DofOpenSim4::GetLimitMoment() const
	{
		if ( m_pOsLimitForce )
			return m_pOsLimitForce->calcLimitForce( m_Model.GetTkState() );
		else return 0.0;
	}

	void DofOpenSim4::SetPos( Real pos, bool enforce_constraints )
	{
		if ( !m_osCoord.getLocked( m_Model.GetTkState() ) )
			m_osCoord.setValue( m_Model.GetTkState(), pos, enforce_constraints );
	}

	void DofOpenSim4::SetVel( Real vel )
	{
		if ( !m_osCoord.getLocked( m_Model.GetTkState() ) )
			m_osCoord.setSpeedValue( m_Model.GetTkState(), vel );
	}

	Vec3 DofOpenSim4::GetRotationAxis() const
	{
		return m_RotationAxis;
	}

	Range< Real > DofOpenSim4::GetRange() const
	{
		return Range< Real >( m_osCoord.get_range( 0 ), m_osCoord.get_range( 1 ) );
	}

	Real DofOpenSim4::GetMinInput() const
	{
		return m_OsCoordAct ? m_OsCoordAct->getMinControl() : 0.0;
	}

	Real DofOpenSim4::GetMaxInput() const
	{
		return m_OsCoordAct ? m_OsCoordAct->getMaxControl() : 0.0;
	}

	Real DofOpenSim4::GetMinTorque() const
	{
		return m_OsCoordAct ? m_OsCoordAct->getMinControl() * m_OsCoordAct->getOptimalForce() : 0.0;
	}

	Real DofOpenSim4::GetMaxTorque() const
	{
		return m_OsCoordAct ? m_OsCoordAct->getMaxControl() * m_OsCoordAct->getOptimalForce() : 0.0;
	}

	const Model& DofOpenSim4::GetModel() const
	{
		return m_Model;
	}
}
