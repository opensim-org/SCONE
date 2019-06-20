#include "ContactForceOpenSim3.h"

#include "OpenSim/Simulation/Model/Force.h"

namespace scone
{
	ContactForceOpenSim3::ContactForceOpenSim3( class ModelOpenSim3& model, OpenSim::Force& force ) :
		m_osForce( force )
	{
	}

	ContactForceOpenSim3::~ContactForceOpenSim3()
	{}

	const String& ContactForceOpenSim3::GetName() const
	{
		return m_osForce.getName();
	}

	Vec3 ContactForceOpenSim3::GetForce() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

	Vec3 ContactForceOpenSim3::GetMoment() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}

	Vec3 ContactForceOpenSim3::GetPoint() const
	{
		throw std::logic_error( "The method or operation is not implemented." );
	}
}
