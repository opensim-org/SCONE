/*
** BodySimbody.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "BodySimbody.h"
#include "scone/core/Exception.h"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Actuators/PointActuator.h>

#include "ModelSimbody.h"
#include "scone/core/Profiler.h"
#include "simbody_tools.h"
#include "scone/core/Log.h"
#include "OpenSim/Common/Geometry.h"

namespace scone
{
	BodySimbody::BodySimbody( class ModelSimbody& model, OpenSim::Body& body ) :
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

	const String& BodySimbody::GetName() const
	{
		return m_osBody.getName();
	}

	scone::Vec3 scone::BodySimbody::GetOriginPos() const
	{
		SCONE_PROFILE_FUNCTION;
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 point;

		m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, zero, point );
		return from_osim( point );
	}

	scone::Vec3 scone::BodySimbody::GetComPos() const
	{
		SCONE_PROFILE_FUNCTION;
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		// TODO: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 com;
		SimTK::Vec3 point;

		// TODO: validate this!
		m_osBody.getMassCenter( com );
		m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, com, point );
		return from_osim( point );
	}

	scone::Vec3 BodySimbody::GetLocalComPos() const
	{
		return m_LocalComPos;
	}

	scone::Quat scone::BodySimbody::GetOrientation() const
	{
		// TODO: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		const auto& quat = mb.getBodyRotation( m_Model.GetTkState() ).convertRotationToQuaternion();
		Quat q1( quat[ 0 ], quat[ 1 ], quat[ 2 ], quat[ 3 ] );

		return q1;
	}

	scone::Vec3 scone::BodySimbody::GetPosOfPointOnBody( Vec3 point ) const
	{
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.findStationLocationInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}

	scone::Vec3 scone::BodySimbody::GetComVel() const
	{
		SCONE_PROFILE_FUNCTION;
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// TODO: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 com;
		SimTK::Vec3 vel;

		// TODO: validate this!
		m_osBody.getMassCenter( com );
		m_osBody.getModel().getSimbodyEngine().getVelocity( m_Model.GetTkState(), m_osBody, com, vel );
		return from_osim( vel );
	}

	scone::Vec3 scone::BodySimbody::GetOriginVel() const
	{
		SCONE_PROFILE_FUNCTION;

		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// TODO: OSIM: see if we can do this more efficient
		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.getBodyOriginVelocity( m_Model.GetTkState() ) );
	}

	scone::Vec3 scone::BodySimbody::GetAngVel() const
	{
		SCONE_PROFILE_FUNCTION;

		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// TODO: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mb.getBodyAngularVelocity( m_Model.GetTkState() ) );
	}

	scone::Vec3 scone::BodySimbody::GetLinVelOfPointOnBody( Vec3 point ) const
	{
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.findStationVelocityInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}


	scone::Vec3 scone::BodySimbody::GetComAcc() const
	{
		SCONE_PROFILE_FUNCTION;
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// TODO: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 com;
		SimTK::Vec3 acc;

		// TODO: validate this!
		m_osBody.getMassCenter( com );
		m_osBody.getModel().getSimbodyEngine().getAcceleration( m_Model.GetTkState(), m_osBody, com, acc );
		return from_osim( acc );
	}

	scone::Vec3 scone::BodySimbody::GetOriginAcc() const
	{
		SCONE_PROFILE_FUNCTION;

		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// TODO: OSIM: see if we can do this more efficient
		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.getBodyOriginAcceleration( m_Model.GetTkState() ) );
	}

	scone::Vec3 scone::BodySimbody::GetAngAcc() const
	{
		SCONE_PROFILE_FUNCTION;

		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// TODO: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mb.getBodyAngularAcceleration( m_Model.GetTkState() ) );
	}

	scone::Vec3 scone::BodySimbody::GetLinAccOfPointOnBody( Vec3 point ) const
	{
		// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
		return from_osim( mob.findStationAccelerationInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}

	Vec3 BodySimbody::GetContactForce() const
	{
		SCONE_PROFILE_FUNCTION;
		if ( m_ForceIndex != -1 )
		{
			const auto& f = GetContactForceValues();
			return Vec3( -f[ 0 ], -f[ 1 ], -f[ 2 ] ); // entry 0-2 are forces applied to ground
		}
		else return Vec3::zero();
	}

	Vec3 BodySimbody::GetContactMoment() const
	{
		if ( m_ForceIndex != -1 )
		{
			const auto& f = GetContactForceValues();
			return Vec3( -f[ 3 ], -f[ 4 ], -f[ 5 ] ); // entry 3-5 are moments applied to ground
		}
		else return Vec3::zero();
	}

	void BodySimbody::ConnectContactForce( const String& force_name )
	{
		m_ForceIndex = m_osBody.getModel().getForceSet().getIndex( force_name, 0 );
		if ( m_ForceIndex != -1 )
		{
			auto labels = m_osBody.getModel().getForceSet().get( m_ForceIndex ).getRecordLabels();
			for ( int i = 0; i < labels.size(); ++i )
				m_ContactForceLabels.push_back( labels[ i ] );
			m_ContactForceValues.resize( m_ContactForceLabels.size() );
		}
	}

	const Model& BodySimbody::GetModel() const
	{
		return dynamic_cast<const Model&>( m_Model );
	}

	Model& BodySimbody::GetModel()
	{
		return dynamic_cast<Model&>( m_Model );
	}

	std::vector< DisplayGeometry > BodySimbody::GetDisplayGeometries() const
	{
		std::vector< DisplayGeometry > geoms;

		auto& gset = m_osBody.getDisplayer()->getGeometrySet();
		for ( auto i = 0; i < gset.getSize(); ++i )
		{
			geoms.emplace_back();
			geoms.back().filename = gset[ i ].getGeometryFile();
			geoms.back().pos = from_osim( gset[ i ].getTransform().p() );
			geoms.back().ori = from_osim( SimTK::Quaternion( gset[ i ].getTransform().R() ) );
			geoms.back().scale = from_osim( gset[ i ].getScaleFactors() );
		}

		return geoms;
	}

	const std::vector< scone::Real >& BodySimbody::GetContactForceValues() const
	{
		if ( m_ForceIndex != -1 )
		{
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
			int num_dyn = m_osBody.getModel().getMultibodySystem().getNumRealizationsOfThisStage( SimTK::Stage::Dynamics );

			if ( m_LastNumDynamicsRealizations != num_dyn )
			{
				// TODO: find out if this can be done less clumsy in OpenSim
				m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
				OpenSim::Array<double> forces = m_osBody.getModel().getForceSet().get( m_ForceIndex ).getRecordValues( m_Model.GetTkState() );
				for ( int i = 0; i < forces.size(); ++i )
					m_ContactForceValues[ i ] = forces[ i ];
				m_LastNumDynamicsRealizations = num_dyn;
			}
		}
		return m_ContactForceValues;
	}

	void BodySimbody::SetExternalForce( const Vec3& f )
	{
		SetExternalForceAtPoint( f, m_LocalComPos );
	}

	void BodySimbody::SetExternalForceAtPoint( const Vec3& force, const Vec3& point )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setForceAtPoint( to_osim( force ), to_osim( point ) );
	}

	void BodySimbody::SetExternalMoment( const Vec3& torque )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setTorque( to_osim( torque ) );
	}

	void BodySimbody::AddExternalForce( const Vec3& force )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setForce( to_osim( force ) + cf->getForce() );
	}

	void BodySimbody::AddExternalMoment( const Vec3& torque )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			cf->setTorque( to_osim( torque ) + cf->getTorque() );
	}

	scone::Vec3 BodySimbody::GetExternalForce() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			return from_osim( cf->getForce() );
		else return Vec3::zero();
	}

	scone::Vec3 BodySimbody::GetExternalForcePoint() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			return from_osim( cf->getPoint() );
		else return Vec3::zero();
	}

	scone::Vec3 BodySimbody::GetExternalMoment() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() ) )
			return from_osim( cf->getTorque() );
		else return Vec3::zero();
	}
}
