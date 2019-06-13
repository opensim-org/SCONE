/*
** BodyOpenSim3.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "BodyOpenSim3.h"
#include "scone/core/Exception.h"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Actuators/PointActuator.h>

#include "ModelOpenSim3.h"
#include "ConstantForce.h"
#include "scone/core/profiler_config.h"
#include "simbody_tools.h"
#include "scone/core/Log.h"
#include "OpenSim/Common/Geometry.h"

namespace scone
{
	BodyOpenSim3::BodyOpenSim3( class ModelOpenSim3& model, OpenSim::Body& body ) :
		Body(),
		m_osBody( body ),
		m_Model( model ),
		m_ForceIndex( -1 ),
		m_LastNumDynamicsRealizations( -1 )
	{
		ConnectContactForce( body.getName() );
		SimTK::Vec3 com;
		m_osBody.getMassCenter( com );
		m_LocalComPos = from_osim( com );
	}

	const String& BodyOpenSim3::GetName() const
	{
		return m_osBody.getName();
	}

	Vec3 BodyOpenSim3::GetOriginPos() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 point;

		m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, zero, point );
		return from_osim( point );
	}

	Vec3 BodyOpenSim3::GetComPos() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		// #todo: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 com;
		SimTK::Vec3 point;

		// #todo: validate this!
		m_osBody.getMassCenter( com );
		m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, com, point );
		return from_osim( point );
	}

	Vec3 BodyOpenSim3::GetLocalComPos() const
	{
		return m_LocalComPos;
	}

	Quat BodyOpenSim3::GetOrientation() const
	{
		// #todo: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		const auto& quat = mb.getBodyRotation( m_Model.GetTkState() ).convertRotationToQuaternion();
		Quat q1( quat[ 0 ], quat[ 1 ], quat[ 2 ], quat[ 3 ] );

		return q1;
	}

	Vec3 BodyOpenSim3::GetPosOfPointOnBody( Vec3 point ) const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.findStationLocationInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}

	Vec3 BodyOpenSim3::GetComVel() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// #todo: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 com;
		SimTK::Vec3 vel;

		// #todo: validate this!
		m_osBody.getMassCenter( com );
		m_osBody.getModel().getSimbodyEngine().getVelocity( m_Model.GetTkState(), m_osBody, com, vel );
		return from_osim( vel );
	}

	Vec3 BodyOpenSim3::GetOriginVel() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// #todo: OSIM: see if we can do this more efficient
		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.getBodyOriginVelocity( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim3::GetAngVel() const
	{

		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// #todo: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mb.getBodyAngularVelocity( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim3::GetLinVelOfPointOnBody( Vec3 point ) const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.findStationVelocityInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}


	Vec3 BodyOpenSim3::GetComAcc() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// #todo: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 com;
		SimTK::Vec3 acc;

		// #todo: validate this!
		m_osBody.getMassCenter( com );
		m_osBody.getModel().getSimbodyEngine().getAcceleration( m_Model.GetTkState(), m_osBody, com, acc );
		return from_osim( acc );
	}

	Vec3 BodyOpenSim3::GetOriginAcc() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// #todo: OSIM: see if we can do this more efficient
		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.getBodyOriginAcceleration( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim3::GetAngAcc() const
	{

		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// #todo: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mb.getBodyAngularAcceleration( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim3::GetLinAccOfPointOnBody( Vec3 point ) const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.findStationAccelerationInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}

	Vec3 BodyOpenSim3::GetContactForce() const
	{
		// #issue55: change this into a generic form
		if ( m_ForceIndex != -1 )
		{
			const auto& f = GetContactForceValues();
			return Vec3( -f[ 0 ], -f[ 1 ], -f[ 2 ] ); // entry 0-2 are forces applied to ground
		}
		else return Vec3::zero();
	}

	Vec3 BodyOpenSim3::GetContactMoment() const
	{
		// #issue55: change this into a generic form
		if ( m_ForceIndex != -1 )
		{
			const auto& f = GetContactForceValues();
			return Vec3( -f[ 3 ], -f[ 4 ], -f[ 5 ] ); // entry 3-5 are moments applied to ground
		}
		else return Vec3::zero();
	}

	void BodyOpenSim3::ConnectContactForce( const String& force_name )
	{
		const auto& forces = m_osBody.getModel().getForceSet();
		for ( int i = 0; i < forces.getSize(); ++i )
		{
			if ( auto* hcf = dynamic_cast<const OpenSim::HuntCrossleyForce*>( &forces.get( i ) ) )
			{
				// #issue55: find a way to assign the correct force and index here
			}
		}

		// #todo: this code can be removed once #issue55 is resolved
		m_ForceIndex = forces.getIndex( force_name, 0 );
		if ( m_ForceIndex != -1 )
		{
			auto labels = forces.get( m_ForceIndex ).getRecordLabels();
			for ( int i = 0; i < labels.size(); ++i )
				m_ContactForceLabels.push_back( labels[ i ] );
			m_ContactForceValues.resize( m_ContactForceLabels.size() );
		}
	}

	const Model& BodyOpenSim3::GetModel() const
	{
		return dynamic_cast<const Model&>( m_Model );
	}

	Model& BodyOpenSim3::GetModel()
	{
		return dynamic_cast<Model&>( m_Model );
	}

	std::vector< DisplayGeometry > BodyOpenSim3::GetDisplayGeometries() const
	{
		std::vector< DisplayGeometry > geoms;

		auto* disp = m_osBody.getDisplayer();
		auto disp_pos = from_osim( disp->getTransform().p() );
		auto disp_ori = from_osim( SimTK::Quaternion( disp->getTransform().R() ) );
		SimTK::Vec3 disp_scale_tk;
		disp->getScaleFactors( disp_scale_tk );
		auto disp_scale = from_osim( disp_scale_tk );

		auto& gset = disp->getGeometrySet();
		for ( auto i = 0; i < gset.getSize(); ++i )
		{
			geoms.emplace_back();
			geoms.back().filename = gset[ i ].getGeometryFile();
			geoms.back().pos = disp_pos + from_osim( gset[ i ].getTransform().p() );
			geoms.back().ori = disp_ori * from_osim( SimTK::Quaternion( gset[ i ].getTransform().R() ) );
			geoms.back().scale = disp_scale * from_osim( gset[ i ].getScaleFactors() );
		}

		return geoms;
	}

	const std::vector< Real >& BodyOpenSim3::GetContactForceValues() const
	{
		// #issue55: verify this code
		if ( m_ForceIndex != -1 )
		{
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
			int num_dyn = m_osBody.getModel().getMultibodySystem().getNumRealizationsOfThisStage( SimTK::Stage::Dynamics );

			// update m_ContactForceValues only if needed (performance)
			if ( m_LastNumDynamicsRealizations != num_dyn )
			{
				// #todo: find out if this can be done less clumsy in OpenSim
				m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
				OpenSim::Array<double> forces = m_osBody.getModel().getForceSet().get( m_ForceIndex ).getRecordValues( m_Model.GetTkState() );
				for ( int i = 0; i < forces.size(); ++i )
					m_ContactForceValues[ i ] = forces[ i ];
				m_LastNumDynamicsRealizations = num_dyn;
			}
		}

		return m_ContactForceValues;
	}

	void BodyOpenSim3::SetExternalForce( const Vec3& f )
	{
		SetExternalForceAtPoint( f, m_LocalComPos );
	}

	void BodyOpenSim3::SetExternalForceAtPoint( const Vec3& force, const Vec3& point )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setForceAtPoint( to_osim( force ), to_osim( point ) );
	}

	void BodyOpenSim3::SetExternalMoment( const Vec3& torque )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setTorque( to_osim( torque ) );
	}

	void BodyOpenSim3::AddExternalForce( const Vec3& force )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setForce( to_osim( force ) + cf->getForce() );
	}

	void BodyOpenSim3::AddExternalMoment( const Vec3& torque )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setTorque( to_osim( torque ) + cf->getTorque() );
	}

	Vec3 BodyOpenSim3::GetExternalForce() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			return from_osim( cf->getForce() );
		else return Vec3::zero();
	}

	Vec3 BodyOpenSim3::GetExternalForcePoint() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			return from_osim( cf->getPoint() );
		else return Vec3::zero();
	}

	Vec3 BodyOpenSim3::GetExternalMoment() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			return from_osim( cf->getTorque() );
		else return Vec3::zero();
	}
}
