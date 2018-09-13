#include "JointSimbody.h"
#include "scone/core/Exception.h"
#include "scone/core/Log.h"
#include "ModelSimbody.h"

#include <OpenSim/OpenSim.h>
#include "simbody_tools.h"

namespace scone
{
	JointSimbody::JointSimbody( Body& body, Joint* parent, class ModelSimbody& model, OpenSim::Joint& osJoint ) :
		Joint( body, parent ),
		m_osJoint( osJoint ),
		m_Model( model )
	{
		//log::Trace( "Creating joint " + osJoint.getName() + " body=" + body.GetName() + " parent=" + ( parent ? parent->GetName() : "null" ) );
	}

	JointSimbody::~JointSimbody()
	{
	}

	const String& JointSimbody::GetName() const
	{
		return m_osJoint.getName();
	}

	size_t JointSimbody::GetDofCount() const
	{
		return m_osJoint.numCoordinates();
	}

	scone::Real JointSimbody::GetDofValue( size_t index /*= 0 */ ) const
	{
		return m_osJoint.getCoordinateSet().get( static_cast<int>( index ) ).getValue( m_Model.GetTkState() );
	}

	const String& JointSimbody::GetDofName( size_t index /*= 0 */ ) const
	{
		return m_osJoint.getCoordinateSet().get( static_cast<int>( index ) ).getName();
	}

	scone::Vec3 JointSimbody::GetReactionForce() const
	{
		auto& model = m_osJoint.getModel();
		auto& matter = model.getMatterSubsystem();
		auto& state = m_Model.GetTkState();
		auto child_body_idx = m_osJoint.getBody().getIndex();

		SimTK::Vector_< SimTK::SpatialVec > forcesAtMInG;
		matter.calcMobilizerReactionForces( state, forcesAtMInG ); // state should be at acceleration

#if 1
		return ToVec3( forcesAtMInG[ child_body_idx ][ 1 ] );
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

		return ToVec3( forcesAtFInG[ 1 ] );
#endif
	}

	Vec3 JointSimbody::GetPos() const
	{
		SCONE_THROW_NOT_IMPLEMENTED;

		// TODO: compute the actual world pos of this joint instead of the pos in the parent frame
		// OpenSim: how can we get the actual position of a joint
		SimTK::Vec3 p;
		m_osJoint.getLocationInParent( p );
		std::cout << GetName() << ": " << p << std::endl;
		return ToVec3( p );
	}
}
