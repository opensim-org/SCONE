/*
** MuscleSimbody.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include <OpenSim/OpenSim.h>

#include "MuscleSimbody.h"
#include "ModelSimbody.h"

#include "scone/core/Exception.h"
#include "scone/core/Profiler.h"

#include "DofSimbody.h"
#include "simbody_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	const double MOMENT_ARM_EPSILON = 0.000001;

	MuscleSimbody::MuscleSimbody( ModelSimbody& model, OpenSim::Muscle& mus ) : m_Model( model ), m_osMus( mus )
	{}

	MuscleSimbody::~MuscleSimbody()
	{}

	const String& MuscleSimbody::GetName() const
	{
		return m_osMus.getName();
	}

	scone::Real MuscleSimbody::GetOptimalFiberLength() const
	{
		return m_osMus.getOptimalFiberLength();
	}

	scone::Real MuscleSimbody::GetTendonSlackLength() const
	{
		return m_osMus.getTendonSlackLength();
	}

	scone::Real MuscleSimbody::GetMass( Real specific_tension, Real muscle_density ) const
	{
		return ( GetMaxIsometricForce() / specific_tension ) * muscle_density * GetOptimalFiberLength(); // from OpenSim Umberger metabolic energy model docs
	}

	scone::Real MuscleSimbody::GetForce() const
	{
		SCONE_PROFILE_FUNCTION;
		// OpenSim: why can't I just use getWorkingState()?
		// OpenSim: why must I update to Dynamics for getForce()?
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
		// TODO is this correct?
		return m_osMus.getActuation( m_Model.GetTkState() );

	}

	scone::Real MuscleSimbody::GetNormalizedForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return GetForce() / GetMaxIsometricForce();
	}

	scone::Real scone::MuscleSimbody::GetLength() const
	{
		SCONE_PROFILE_FUNCTION;
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
		return m_osMus.getLength( m_Model.GetTkState() );
	}

	scone::Real MuscleSimbody::GetVelocity() const
	{
		SCONE_PROFILE_FUNCTION;
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
		return m_osMus.getLengtheningSpeed( m_Model.GetTkState() );
	}

	scone::Real MuscleSimbody::GetFiberForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberForce( m_Model.GetTkState() );
	}

	scone::Real MuscleSimbody::GetNormalizedFiberForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberForce( m_Model.GetTkState() ) / m_osMus.getMaxIsometricForce();
	}

	scone::Real MuscleSimbody::GetActiveFiberForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getActiveFiberForce( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleSimbody::GetFiberLength() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberLength( m_Model.GetTkState() );
	}

	scone::Real MuscleSimbody::GetNormalizedFiberLength() const
	{
		SCONE_PROFILE_FUNCTION;
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
		return m_osMus.getNormalizedFiberLength( m_Model.GetTkState() );
	}

	scone::Real MuscleSimbody::GetFiberVelocity() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberVelocity( m_Model.GetTkState() );
	}

	scone::Real MuscleSimbody::GetNormalizedFiberVelocity() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberVelocity( m_Model.GetTkState() ) / m_osMus.getOptimalFiberLength();
	}

	const Link& MuscleSimbody::GetOriginLink() const
	{
		SCONE_PROFILE_FUNCTION;
		auto& pps = m_osMus.getGeometryPath().getPathPointSet();
		return m_Model.FindLink( pps.get( 0 ).getBodyName() );
	}

	const Link& MuscleSimbody::GetInsertionLink() const
	{
		SCONE_PROFILE_FUNCTION;
		auto& pps = m_osMus.getGeometryPath().getPathPointSet();
		return m_Model.FindLink( pps.get( pps.getSize() - 1 ).getBodyName() );
	}

	scone::Real MuscleSimbody::GetMomentArm( const Dof& dof ) const
	{
		SCONE_PROFILE_FUNCTION;

		auto iter = m_MomentArmCache.find( &dof );
		if ( iter == m_MomentArmCache.end() )
		{
			const DofSimbody& dof_sb = dynamic_cast<const DofSimbody&>( dof );
			auto moment = m_osMus.getGeometryPath().computeMomentArm( m_Model.GetTkState(), dof_sb.GetOsCoordinate() );
			if ( fabs( moment ) < MOMENT_ARM_EPSILON || dof_sb.GetOsCoordinate().getLocked( m_Model.GetTkState() ) )
				moment = 0;
			m_MomentArmCache[ &dof ] = moment;
			return moment;
		}
		else return iter->second;
	}

	const scone::Model& MuscleSimbody::GetModel() const
	{
		return m_Model;
	}

	scone::Real scone::MuscleSimbody::GetTendonLength() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getTendonLength( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleSimbody::GetActiveForceLengthMultipler() const
	{
		return m_osMus.getActiveForceLengthMultiplier( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleSimbody::GetMaxContractionVelocity() const
	{
		return m_osMus.getMaxContractionVelocity();
	}

	scone::Real scone::MuscleSimbody::GetMaxIsometricForce() const
	{
		return m_osMus.getMaxIsometricForce();
	}

	std::vector< Vec3 > scone::MuscleSimbody::GetMusclePath() const
	{
		SCONE_PROFILE_FUNCTION;
		//m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
		//m_osMus.getGeometryPath().updateGeometry( m_Model.GetTkState() );
		auto& pps = m_osMus.getGeometryPath().getCurrentPath( m_Model.GetTkState() );
		std::vector< Vec3 > points( pps.getSize() );
		for ( int i = 0; i < points.size(); ++i )
		{
			const auto& mob = m_Model.GetOsimModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( pps[ i ]->getBody().getMobilizedBodyIndex() );
			auto world_pos = mob.getBodyTransform( m_Model.GetTkState() ) * pps[ i ]->getLocation( m_Model.GetTkState() );
			points[ i ] = ToVec3( world_pos );
		}
		return points;
	}

	scone::Real scone::MuscleSimbody::GetActivation() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getActivation( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleSimbody::GetExcitation() const
	{
		// use our own control value, as OpenSim calls getControls()
		// this could lead to infinite recursion
		// make sure to clamp it for calls (important for metabolics)
		return xo::clamped( GetInput(), 0.0, 1.0 );
	}

	void scone::MuscleSimbody::SetExcitation( Real u )
	{
		m_osMus.setExcitation( m_Model.GetTkState(), u );
	}
}
