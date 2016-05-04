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
