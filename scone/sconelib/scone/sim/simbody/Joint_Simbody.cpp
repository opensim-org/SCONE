#include "Joint_Simbody.h"
#include "scone/core/Exception.h"
#include "Model_Simbody.h"

#include <OpenSim/OpenSim.h>
#include "tools.h"

namespace scone
{
	namespace sim
	{
		Joint_Simbody::Joint_Simbody( Body& body, Joint* parent, class Model_Simbody& model, OpenSim::Joint& osJoint ) :
		Joint( body, parent ),
		m_osJoint( osJoint ),
		m_Model( model )
		{
			log::Trace( "Creating joint " + osJoint.getName() + " body=" + body.GetName() + " parent=" + ( parent ? parent->GetName() : "null" ) );
		}

		Joint_Simbody::~Joint_Simbody()
		{
		}

		const String& Joint_Simbody::GetName() const
		{
			return m_osJoint.getName();
		}

		size_t Joint_Simbody::GetDofCount() const
		{
			return m_osJoint.numCoordinates();
		}

		scone::Real Joint_Simbody::GetDof( size_t index /*= 0 */ ) const
		{
			return m_osJoint.getCoordinateSet().get( index ).getValue( m_Model.GetTkState() );
		}

		const String& Joint_Simbody::GetDofName( size_t index /*= 0 */ ) const
		{
			return m_osJoint.getCoordinateSet().get( index ).getName();
		}

		scone::Vec3 Joint_Simbody::GetReactionForce() const
		{
			auto& matter = m_osJoint.getModel().getMatterSubsystem();
			auto& state = m_Model.GetTkState();
			SimTK::Vector_< SimTK::SpatialVec > forcesAtMInG;
			matter.calcMobilizerReactionForces( state, forcesAtMInG );

			auto mbidx = m_osJoint.getBody().getIndex();

		    const SimTK::MobilizedBody& mobod  = matter.getMobilizedBody(mbidx);
		    const SimTK::MobilizedBody& parent = mobod.getParentMobilizedBody();

			// Want to shift reaction by p_MF, the vector from M to F across the
			// mobilizer, and negate. Can get p_FM; must reexpress in G.
			const SimTK::Vec3& p_FM = mobod.getMobilizerTransform(state).p();
			const SimTK::Rotation& R_PF = mobod.getInboardFrame(state).R(); // In parent.
			const SimTK::Rotation& R_GP = parent.getBodyTransform(state).R();
			SimTK::Rotation R_GF   =   R_GP*R_PF;  // F frame orientation in Ground.
			SimTK::Vec3 p_MF_G = -( R_GF * p_FM ); // Re-express and negate shift vector. 
			SimTK::SpatialVec forcesAtFInG = -SimTK::shiftForceBy( forcesAtMInG[ mbidx ], p_MF_G );

			return ToVec3( forcesAtFInG[ 1 ] );
		}

		Vec3 Joint_Simbody::GetPos() const
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
}
