#include "stdafx.h"

#include "Joint_Simbody.h"
#include "../../core/Exception.h"
#include "Model_Simbody.h"

#include <OpenSim/OpenSim.h>

namespace scone
{
	namespace sim
	{
		Joint_Simbody::Joint_Simbody( class Model_Simbody& model, OpenSim::Joint& osJoint ) : m_osJoint( osJoint ), m_Model( model )
		{
		}

		Joint_Simbody::~Joint_Simbody()
		{
		}

		const String& Joint_Simbody::GetName() const
		{
			return m_osJoint.getName();
		}

		size_t Joint_Simbody::GetDofCount()
		{
			return m_osJoint.numCoordinates();
		}

		scone::Real Joint_Simbody::GetDof( size_t index /*= 0 */ )
		{
			return m_osJoint.getCoordinateSet().get( index ).getValue( m_Model.GetTkState() );
		}

		const String& Joint_Simbody::GetDofName( size_t index /*= 0 */ )
		{
			return m_osJoint.getCoordinateSet().get( index ).getName();
		}
	}
}
