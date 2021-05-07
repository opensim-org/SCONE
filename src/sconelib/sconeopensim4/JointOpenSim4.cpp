/*
** JointOpenSim4.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "JointOpenSim4.h"
#include "scone/core/Exception.h"
#include "scone/core/Log.h"
#include "ModelOpenSim4.h"
#include "scone/model/Dof.h"

#include <OpenSim/Simulation/SimbodyEngine/Joint.h>
#include <OpenSim/Simulation/SimbodyEngine/Body.h>
#include <OpenSim/Simulation/Model/Model.h>

#include "simbody_tools.h"

namespace scone
{
	JointOpenSim4::JointOpenSim4( Body& body, Body& parent, class ModelOpenSim4& model, OpenSim::Joint& osJoint ) :
		Joint( body, parent ),
		m_Model( model ),
		m_osJoint( osJoint )
	{
		//log::Trace( "Creating joint " + osJoint.getName() + " body=" + body.GetName() + " parent=" + ( parent ? parent->GetName() : "null" ) );
	}

	JointOpenSim4::~JointOpenSim4()
	{}

	const String& JointOpenSim4::GetName() const
	{
		return m_osJoint.getName();
	}

	Vec3 JointOpenSim4::GetReactionForce() const
	{
		auto& model = m_osJoint.getModel();
		auto& matter = model.getMatterSubsystem();
		auto& state = m_Model.GetTkState();
		auto child_body_idx = m_osJoint.getChildFrame().getMobilizedBodyIndex();

		SimTK::Vector_< SimTK::SpatialVec > forcesAtMInG;
		matter.calcMobilizerReactionForces( state, forcesAtMInG ); // state should be at acceleration

#if 1
		return from_osim( forcesAtMInG[ child_body_idx ][ 1 ] );
#else
		const SimTK::MobilizedBody& mobod = matter.getMobilizedBody( child_body_idx );
		const SimTK::MobilizedBody& parent = mobod.getParentMobilizedBody();

		// Want to shift reaction by p_MF, the vector from M to F across the
		// mobilizer, and negate. Can get p_FM; must reexpress in G.
		const SimTK::Vec3& p_FM = mobod.getMobilizerTransform( state ).p();
		const SimTK::Rotation& R_PF = mobod.getInboardFrame( state ).R(); // In parent.
		const SimTK::Rotation& R_GP = parent.getBodyTransform( state ).R();
		SimTK::Rotation R_GF = R_GP * R_PF;  // F frame orientation in Ground.
		SimTK::Vec3 p_MF_G = -( R_GF * p_FM ); // Re-express and negate shift vector. 
		SimTK::SpatialVec forcesAtFInG = -SimTK::shiftForceBy( forcesAtMInG[ child_body_idx ], p_MF_G );

		return from_osim( forcesAtFInG[ 1 ] );
#endif
	}

	Real JointOpenSim4::GetLimitPower() const
	{
		Real pow = 0.0;
		for ( const auto& d : GetDofs() )
			pow += d->GetVel() * d->GetLimitMoment();
		return pow;
	}

	Vec3 JointOpenSim4::GetPos() const
	{
#if 1
		// #osim4: this doesn't work for the fancy knee joints
		auto point = m_osJoint.getParentFrame().getPositionInGround( m_Model.GetTkState() );
		return from_osim( point );
#else
		// #osim4: this code gives an error
		auto ofs = SimTK::Vec3( 0, 0, 0 );
		SimTK::Vec3 point;
		const auto& body_frame = m_osJoint.getChildFrame();
		const auto& joint_frame = m_osJoint.getChildFrame();
		m_osJoint.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), joint_frame, ofs, point );
		getPosition( m_Model.GetTkState(), m_osJoint.getBody(), m_osJoint.getLocationInChild(), point );
		return from_osim( point );
#endif
	}
}
