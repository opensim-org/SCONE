/*
** MuscleOpenSim3.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include <OpenSim/OpenSim.h>

#include "MuscleOpenSim3.h"
#include "ModelOpenSim3.h"

#include "scone/core/Exception.h"
#include "scone/core/Profiler.h"

#include "DofOpenSim3.h"
#include "simbody_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	const double MOMENT_ARM_EPSILON = 0.000001;

	MuscleOpenSim3::MuscleOpenSim3( ModelOpenSim3& model, OpenSim::Muscle& mus ) : m_Model( model ), m_osMus( mus )
	{}

	MuscleOpenSim3::~MuscleOpenSim3()
	{}

	const String& MuscleOpenSim3::GetName() const
	{
		return m_osMus.getName();
	}

	scone::Real MuscleOpenSim3::GetOptimalFiberLength() const
	{
		return m_osMus.getOptimalFiberLength();
	}

	scone::Real MuscleOpenSim3::GetTendonSlackLength() const
	{
		return m_osMus.getTendonSlackLength();
	}

	scone::Real MuscleOpenSim3::GetForce() const
	{
		SCONE_PROFILE_FUNCTION;
		// OpenSim: why can't I just use getWorkingState()?
		// OpenSim: why must I update to Dynamics for getForce()?
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
		return m_osMus.getForce( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetNormalizedForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return GetForce() / GetMaxIsometricForce();
	}

	scone::Real scone::MuscleOpenSim3::GetLength() const
	{
		SCONE_PROFILE_FUNCTION;
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
		return m_osMus.getLength( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetVelocity() const
	{
		SCONE_PROFILE_FUNCTION;
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
		return m_osMus.getLengtheningSpeed( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetFiberForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberForce( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetNormalizedFiberForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberForce( m_Model.GetTkState() ) / m_osMus.getMaxIsometricForce();
	}

	scone::Real MuscleOpenSim3::GetActiveFiberForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getActiveFiberForce( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleOpenSim3::GetFiberLength() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberLength( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetNormalizedFiberLength() const
	{
		SCONE_PROFILE_FUNCTION;
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
		return m_osMus.getNormalizedFiberLength( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetFiberVelocity() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberVelocity( m_Model.GetTkState() );
	}

	scone::Real MuscleOpenSim3::GetNormalizedFiberVelocity() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getFiberVelocity( m_Model.GetTkState() ) / m_osMus.getOptimalFiberLength();
	}

	const Link& MuscleOpenSim3::GetOriginLink() const
	{
		SCONE_PROFILE_FUNCTION;
		auto& pps = m_osMus.getGeometryPath().getPathPointSet();
		return m_Model.FindLink( pps.get( 0 ).getBodyName() );
	}

	const Link& MuscleOpenSim3::GetInsertionLink() const
	{
		SCONE_PROFILE_FUNCTION;
		auto& pps = m_osMus.getGeometryPath().getPathPointSet();
		return m_Model.FindLink( pps.get( pps.getSize() - 1 ).getBodyName() );
	}

	scone::Real MuscleOpenSim3::GetMomentArm( const Dof& dof ) const
	{
		SCONE_PROFILE_FUNCTION;

		auto iter = m_MomentArmCache.find( &dof );
		if ( iter == m_MomentArmCache.end() )
		{
			const DofOpenSim3& dof_sb = dynamic_cast<const DofOpenSim3&>( dof );
			auto moment = m_osMus.getGeometryPath().computeMomentArm( m_Model.GetTkState(), dof_sb.GetOsCoordinate() );
			if ( fabs( moment ) < MOMENT_ARM_EPSILON || dof_sb.GetOsCoordinate().getLocked( m_Model.GetTkState() ) )
				moment = 0;
			m_MomentArmCache[ &dof ] = moment;
			return moment;
		}
		else return iter->second;
	}

	const scone::Model& MuscleOpenSim3::GetModel() const
	{
		return m_Model;
	}

	scone::Real scone::MuscleOpenSim3::GetTendonLength() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getTendonLength( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleOpenSim3::GetActiveForceLengthMultipler() const
	{
		return m_osMus.getActiveForceLengthMultiplier( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleOpenSim3::GetMaxContractionVelocity() const
	{
		return m_osMus.getMaxContractionVelocity();
	}

	scone::Real scone::MuscleOpenSim3::GetMaxIsometricForce() const
	{
		return m_osMus.getMaxIsometricForce();
	}

	std::vector< Vec3 > scone::MuscleOpenSim3::GetMusclePath() const
	{
		SCONE_PROFILE_FUNCTION;
		//m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
		//m_osMus.getGeometryPath().updateGeometry( m_Model.GetTkState() );
		auto& pps = m_osMus.getGeometryPath().getCurrentPath( m_Model.GetTkState() );
		std::vector< Vec3 > points( pps.getSize() );
		for ( int i = 0; i < points.size(); ++i )
		{
			const auto& mob = m_Model.GetOsimModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( pps[ i ]->getBody().getIndex() );
			auto world_pos = mob.getBodyTransform( m_Model.GetTkState() ) * pps[ i ]->getLocation();
			points[ i ] = from_osim( world_pos );
		}
		return points;
	}

	scone::Real scone::MuscleOpenSim3::GetActivation() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_osMus.getActivation( m_Model.GetTkState() );
	}

	scone::Real scone::MuscleOpenSim3::GetExcitation() const
	{
		// use our own control value, as OpenSim calls getControls()
		// this could lead to infinite recursion
		// make sure to clamp it for calls (important for metabolics)
		return xo::clamped( GetInput(), 0.0, 1.0 );
	}

	void scone::MuscleOpenSim3::SetExcitation( Real u )
	{
		m_osMus.setExcitation( m_Model.GetTkState(), u );
	}
}
