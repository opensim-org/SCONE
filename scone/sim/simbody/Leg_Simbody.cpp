#include "stdafx.h"

#include "Leg_Simbody.h"
#include "Model_Simbody.h"

#include <OpenSim/OpenSim.h>

namespace scone
{
	namespace sim
	{
		Leg_Simbody::Leg_Simbody( class Model_Simbody& model, const Link& upper, const Link& foot, Side side, size_t rank ) :
		Leg( upper, foot, side, rank ),
		m_Model( model )
		{
			// TODO: hopefully this can be done more generic in OpenSim
			const char* force_name = side == LeftSide ? "foot_l" : "foot_r";
			m_ForceIndex = m_Model.GetOsimModel().getForceSet().getIndex( force_name, 0 );
		}

		Leg_Simbody::~Leg_Simbody()
		{
		}

		scone::Vec3 Leg_Simbody::GetContactForce() const
		{
			// OSIM: can this be done without copy?
			m_Model.GetOsimModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
			OpenSim::Array<double> force = m_Model.GetOsimModel().getForceSet().get( m_ForceIndex ).getRecordValues( m_Model.GetTkState() );

			// assume total force is the negative of the first 3 values
			return Vec3( -force[0], -force[1], -force[2] );
		}
	}
}
