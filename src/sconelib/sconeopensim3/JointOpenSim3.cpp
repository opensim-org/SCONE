/*
** JointOpenSim3.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "JointOpenSim3.h"
#include "scone/core/Exception.h"
#include "scone/core/Log.h"
#include "ModelOpenSim3.h"

#include <OpenSim/OpenSim.h>
#include "simbody_tools.h"

namespace scone
{
	JointOpenSim3::JointOpenSim3( Body& body, Joint* parent, class ModelOpenSim3& model, OpenSim::Joint& osJoint ) :
		Joint( body, parent ),
		m_Model( model ),
		m_osJoint( osJoint )
	{
		//log::Trace( "Creating joint " + osJoint.getName() + " body=" + body.GetName() + " parent=" + ( parent ? parent->GetName() : "null" ) );
	}

	JointOpenSim3::~JointOpenSim3()
	{
	}

	const String& JointOpenSim3::GetName() const
	{
		return m_osJoint.getName();
	}

	size_t JointOpenSim3::GetDofCount() const
	{
		return m_osJoint.numCoordinates();
	}

	Real JointOpenSim3::GetDofValue( size_t index ) const
	{
		return m_osJoint.getCoordinateSet().get( static_cast<int>( index ) ).getValue( m_Model.GetTkState() );
	}

	const String& JointOpenSim3::GetDofName( size_t index ) const
	{
		return m_osJoint.getCoordinateSet().get( static_cast<int>( index ) ).getName();
	}

	Vec3 JointOpenSim3::GetReactionForce() const
	{
		auto& model = m_osJoint.getModel();
		auto& matter = model.getMatterSubsystem();
		auto& state = m_Model.GetTkState();
		auto child_body_idx = m_osJoint.getBody().getIndex();

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

	Vec3 JointOpenSim3::GetPos() const
	{
		SimTK::Vec3 point;
		m_osJoint.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osJoint.getBody(), m_osJoint.getLocationInChild(), point );
		return from_osim( point );
		
		// #todo: compute the actual world pos of this joint instead of the pos in the parent frame
		// OpenSim: how can we get the actual position of a joint
	}
}
