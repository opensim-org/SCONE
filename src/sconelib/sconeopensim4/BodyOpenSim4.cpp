/*
** BodyOpenSim4.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "BodyOpenSim4.h"
#include "scone/core/Exception.h"

#include <OpenSim/Simulation/SimbodyEngine/Body.h>
#include <OpenSim/Simulation/Model/Model.h>

#include "ModelOpenSim4.h"
#include "ConstantForce.h"
#include "JointOpenSim4.h"
#include "scone/core/profiler_config.h"
#include "scone/core/Log.h"

#include "simbody_tools.h"

#include <numeric>

namespace scone
{
	BodyOpenSim4::BodyOpenSim4( class ModelOpenSim4& model, const OpenSim::PhysicalFrame& body ) :
		Body(),
		m_osBody( body ),
		m_pBody( dynamic_cast<const OpenSim::Body*>( &m_osBody ) ),
		m_Model( model )
	{
		m_LocalComPos = m_pBody ? from_osim( m_pBody->getMassCenter() ) : Vec3::zero();
	}

	BodyOpenSim4::~BodyOpenSim4()
	{}

	Real BodyOpenSim4::GetMass() const
	{
		return m_pBody ? m_pBody->getMass() : 0.0;
	}

	Vec3 BodyOpenSim4::GetInertiaTensorDiagonal() const
	{
		if ( m_pBody )
		{
			auto inertia = m_pBody->getInertia().toMat33();
			return Vec3( inertia( 0, 0 ), inertia( 1, 1 ), inertia( 2, 2 ) );
		}
		else return Vec3::zero();
	}

	const String& BodyOpenSim4::GetName() const
	{
		return m_osBody.getName();
	}

	Vec3 BodyOpenSim4::GetContactForce() const
	{
		return std::accumulate( m_ContactForces.begin(), m_ContactForces.end(), Vec3::zero(),
			[&]( const Vec3& v, const ContactForce* cf ) { return v + cf->GetForce(); } );
	}

	Vec3 BodyOpenSim4::GetContactMoment() const
	{
		return std::accumulate( m_ContactForces.begin(), m_ContactForces.end(), Vec3::zero(),
			[&]( const Vec3& v, const ContactForce* cf ) { return v + cf->GetMoment(); } );
	}

	Vec3 BodyOpenSim4::GetContactPoint() const
	{
		if ( m_ContactForces.size() >= 2 )
		{
			// weighted average
			Vec3 point = Vec3::zero();
			double total_force = 0.0;
			for ( auto& cf : m_ContactForces )
			{
				auto f = xo::length( cf->GetForce() );
				point += f * cf->GetPoint();
				total_force += f;
			}

			return total_force > 0 ? point / total_force : Vec3::zero();
		}
		else return m_ContactForces.front()->GetPoint();
	}

	Vec3 BodyOpenSim4::GetOriginPos() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
		SimTK::Vec3 point;

		m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, zero, point );
		return from_osim( point );
	}

	Vec3 BodyOpenSim4::GetComPos() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		// #todo: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 com = m_pBody ? m_pBody->getMassCenter() : SimTK::Vec3( 0 );
		SimTK::Vec3 point;

		// #todo: validate this!
		m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, com, point );
		return from_osim( point );
	}

	Vec3 BodyOpenSim4::GetLocalComPos() const
	{
		return m_LocalComPos;
	}

	Quat BodyOpenSim4::GetOrientation() const
	{
		// #todo: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		const auto& quat = mb.getBodyRotation( m_Model.GetTkState() ).convertRotationToQuaternion();
		Quat q1( quat[ 0 ], quat[ 1 ], quat[ 2 ], quat[ 3 ] );

		return q1;
	}

	Vec3 BodyOpenSim4::GetPosOfPointOnBody( Vec3 point ) const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mob.findStationLocationInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}

	Vec3 BodyOpenSim4::GetComVel() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// #todo: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 com = m_pBody ? m_pBody->getMassCenter() : SimTK::Vec3( 0 );
		SimTK::Vec3 vel;

		// #todo: validate this!
		m_osBody.getModel().getSimbodyEngine().getVelocity( m_Model.GetTkState(), m_osBody, com, vel );
		return from_osim( vel );
	}

	Vec3 BodyOpenSim4::GetOriginVel() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// #todo: OSIM: see if we can do this more efficient
		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mob.getBodyOriginVelocity( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim4::GetAngVel() const
	{

		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		// #todo: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mb.getBodyAngularVelocity( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim4::GetLinVelOfPointOnBody( Vec3 point ) const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mob.findStationVelocityInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}


	Vec3 BodyOpenSim4::GetComAcc() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// #todo: OSIM: find what is the most efficient (compare to linvel)
		SimTK::Vec3 com = m_pBody ? m_pBody->getMassCenter() : SimTK::Vec3( 0 );
		SimTK::Vec3 acc;

		// #todo: validate this!
		m_osBody.getModel().getSimbodyEngine().getAcceleration( m_Model.GetTkState(), m_osBody, com, acc );
		return from_osim( acc );
	}

	Vec3 BodyOpenSim4::GetOriginAcc() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// #todo: OSIM: see if we can do this more efficient
		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mob.getBodyOriginAcceleration( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim4::GetAngAcc() const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		// #todo: cache this baby (after profiling), because sensors evaluate it for each channel
		auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mb.getBodyAngularAcceleration( m_Model.GetTkState() ) );
	}

	Vec3 BodyOpenSim4::GetLinAccOfPointOnBody( Vec3 point ) const
	{
		// #todo: see if we need to do this call to realize every time (maybe do it once before controls are updated)
		m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

		const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getMobilizedBodyIndex() );
		return from_osim( mob.findStationAccelerationInGround( m_Model.GetTkState(), SimTK::Vec3( point.x, point.y, point.z ) ) );
	}

	const Model& BodyOpenSim4::GetModel() const
	{
		return dynamic_cast<const Model&>( m_Model );
	}

	Model& BodyOpenSim4::GetModel()
	{
		return dynamic_cast<Model&>( m_Model );
	}

	std::vector< DisplayGeometry > BodyOpenSim4::GetDisplayGeometries() const
	{
		std::vector< DisplayGeometry > geoms;
		for ( const auto& mesh : m_osBody.getComponentList<OpenSim::Mesh>() )
		{
			DisplayGeometry g;
			g.filename = mesh.get_mesh_file();
			g.scale = from_osim( mesh.get_scale_factors() );
			auto trans = mesh.getFrame().findTransformInBaseFrame();
			g.pos = from_osim( trans.p() );
			g.ori = from_osim( SimTK::Quaternion( trans.R() ) );
			geoms.emplace_back( g );
		}
		return geoms;
	}

	void BodyOpenSim4::SetExternalForce( const Vec3& f )
	{
		SetExternalForceAtPoint( f, m_LocalComPos );
	}

	void BodyOpenSim4::SetExternalForceAtPoint( const Vec3& force, const Vec3& point )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			cf->setForceAtPoint( to_osim( force ), to_osim( point ) );
	}

	void BodyOpenSim4::SetExternalMoment( const Vec3& torque )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			cf->setTorque( to_osim( torque ) );
	}

	void BodyOpenSim4::AddExternalForce( const Vec3& force )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			cf->setForce( to_osim( force ) + cf->getForce() );
	}

	void BodyOpenSim4::AddExternalMoment( const Vec3& torque )
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			cf->setTorque( to_osim( torque ) + cf->getTorque() );
	}

	Vec3 BodyOpenSim4::GetExternalForce() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			return from_osim( cf->getForce() );
		else return Vec3::zero();
	}

	Vec3 BodyOpenSim4::GetExternalForcePoint() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			return from_osim( cf->getPoint() );
		else return Vec3::zero();
	}

	Vec3 BodyOpenSim4::GetExternalMoment() const
	{
		if ( auto* cf = m_Model.GetOsimBodyForce( m_osBody.getMobilizedBodyIndex() ) )
			return from_osim( cf->getTorque() );
		else return Vec3::zero();
	}
}
