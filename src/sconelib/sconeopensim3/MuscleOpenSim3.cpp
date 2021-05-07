/*
** MuscleOpenSim3.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include <OpenSim/Simulation/Model/Model.h>
#include <OpenSim/Simulation/Model/Muscle.h>
#include <OpenSim/Simulation/SimbodyEngine/Body.h>
#include <OpenSim/Actuators/Millard2012EquilibriumMuscle.h>
#include <OpenSim/Actuators/Thelen2003Muscle.h>

#include "MuscleOpenSim3.h"
#include "ModelOpenSim3.h"

#include "scone/core/Exception.h"
#include "scone/core/profiler_config.h"

#include "DofOpenSim3.h"
#include "simbody_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	const double MOMENT_ARM_EPSILON = 0.000001;

	MuscleOpenSim3::MuscleOpenSim3( ModelOpenSim3& model, OpenSim::Muscle& mus ) :
		m_Model( model ),
		m_osMus( mus ),
		m_MomentArmCacheTimeStamp( -1 ),
		m_MinActivation( 0 ) // will be set correctly below
	{
		InitJointsDofs();

		// initialize m_MinActivation (#opensim Muscle does not have an interface for this)
		if ( auto mus = dynamic_cast<OpenSim::Millard2012EquilibriumMuscle*>( &m_osMus ) )
			m_MinActivation = mus->getMinimumActivation();
		else if ( auto mus = dynamic_cast<OpenSim::Thelen2003Muscle*>( &m_osMus ) )
			m_MinActivation = mus->getMinimumActivation();
		else m_MinActivation = m_osMus.getMinControl();
	}

	MuscleOpenSim3::~MuscleOpenSim3()
	{}

	const String& MuscleOpenSim3::GetName() const
	{
		return m_osMus.getName();
	}

	Real MuscleOpenSim3::GetOptimalFiberLength() const
	{
		return m_osMus.getOptimalFiberLength();
	}

	Real MuscleOpenSim3::GetTendonSlackLength() const
	{
		return m_osMus.getTendonSlackLength();
	}

	Real MuscleOpenSim3::GetForce() const
	{
		// OpenSim: why can't I just use getWorkingState()?
		// OpenSim: why must I update to Dynamics for getForce()?
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
		return m_osMus.getForce( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetNormalizedForce() const
	{
		return GetForce() / GetMaxIsometricForce();
	}

	Real MuscleOpenSim3::GetLength() const
	{
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
		return m_osMus.getLength( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetVelocity() const
	{
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
		return m_osMus.getLengtheningSpeed( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetFiberForce() const
	{
		return m_osMus.getFiberForce( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetActiveFiberForce() const
	{
		return m_osMus.getActiveFiberForce( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetPassiveFiberForce() const
	{
		return m_osMus.getPassiveFiberForce( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetFiberLength() const
	{
		return m_osMus.getFiberLength( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetNormalizedFiberLength() const
	{
		m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
		return m_osMus.getNormalizedFiberLength( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetCosPennationAngle() const
	{
		return m_osMus.getCosPennationAngle( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetFiberVelocity() const
	{
		return m_osMus.getFiberVelocity( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetNormalizedFiberVelocity() const
	{
		return m_osMus.getFiberVelocity( m_Model.GetTkState() ) / m_osMus.getOptimalFiberLength();
	}

	const Body& MuscleOpenSim3::GetOriginBody() const
	{
		auto& pps = m_osMus.getGeometryPath().getPathPointSet();
		return *FindByName( m_Model.GetBodies(), pps.get( 0 ).getBodyName() );
	}

	const Body& MuscleOpenSim3::GetInsertionBody() const
	{
		auto& pps = m_osMus.getGeometryPath().getPathPointSet();
		return *FindByName( m_Model.GetBodies(), pps.get( pps.getSize() - 1 ).getBodyName() );
	}

	Real MuscleOpenSim3::GetMomentArm( const Dof& dof ) const
	{
		auto t = GetModel().GetTime();
		if ( m_MomentArmCacheTimeStamp != t )
		{
			for ( auto& d : GetDofs() )
			{
				const DofOpenSim3& dof_sb = dynamic_cast<const DofOpenSim3&>( *d );
				auto mom = m_osMus.getGeometryPath().computeMomentArm( m_Model.GetTkState(), dof_sb.GetOsCoordinate() );
				if ( fabs( mom ) < MOMENT_ARM_EPSILON || dof_sb.GetOsCoordinate().getLocked( m_Model.GetTkState() ) )
					mom = 0;
				m_MomentArmCache[ &dof ] = mom;
			}
		}

		return m_MomentArmCache[ &dof ];
	}

	void MuscleOpenSim3::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		Muscle::StoreData( frame, flags );
		if ( flags.get<StoreDataTypes::DebugData>() )
		{
			auto f_t = m_osMus.getTendonForce( m_Model.GetTkState() ) / m_osMus.getCosPennationAngle( m_Model.GetTkState() ) / m_osMus.getMaxIsometricForce();
			auto f_pe = m_osMus.getPassiveFiberForce( m_Model.GetTkState() ) / m_osMus.getMaxIsometricForce();
			auto f_ce = m_osMus.getActiveForceLengthMultiplier( m_Model.GetTkState() ) * m_osMus.getActivation( m_Model.GetTkState() );
			frame[ GetName() + ".inv_ce_vel" ] = ( f_t - f_pe ) / f_ce;
			frame[ GetName() + ".ce_vel_norm" ] = m_osMus.getNormalizedFiberVelocity( m_Model.GetTkState() );
			frame[ GetName() + ".ce_vel" ] = m_osMus.getFiberVelocity( m_Model.GetTkState() );
			frame[ GetName() + ".inv_ce_vel_ft" ] = f_t;
			frame[ GetName() + ".inv_ce_vel_fpe" ] = f_pe;
			frame[ GetName() + ".inv_ce_vel_fce" ] = f_ce;
		}
	}

	const Model& MuscleOpenSim3::GetModel() const
	{
		return m_Model;
	}

	Real MuscleOpenSim3::GetTendonLength() const
	{
		return m_osMus.getTendonLength( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetNormalizedTendonLength() const
	{
		return m_osMus.getTendonLength( m_Model.GetTkState() ) / m_osMus.getTendonSlackLength();
	}

	Real MuscleOpenSim3::GetActiveForceLengthMultipler() const
	{
		return m_osMus.getActiveForceLengthMultiplier( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetMaxContractionVelocity() const
	{
		return m_osMus.getMaxContractionVelocity();
	}

	Real MuscleOpenSim3::GetMaxIsometricForce() const
	{
		return m_osMus.getMaxIsometricForce();
	}

	std::vector< Vec3 > MuscleOpenSim3::GetMusclePath() const
	{
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

	Real MuscleOpenSim3::GetActivation() const
	{
		return m_osMus.getActivation( m_Model.GetTkState() );
	}

	Real MuscleOpenSim3::GetExcitation() const
	{
		// use our own control value, as OpenSim calls getControls()
		// this could lead to infinite recursion
		// make sure to clamp it for calls (important for metabolics)
		return xo::clamped( GetInput(), 0.0, 1.0 );
	}

	void MuscleOpenSim3::SetExcitation( Real u )
	{
		m_osMus.setExcitation( m_Model.GetTkState(), u );
	}
}
