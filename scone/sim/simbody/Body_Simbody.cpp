#include "stdafx.h"
#include "Body_Simbody.h"
#include "../../core/Exception.h"

#include <OpenSim/OpenSim.h>
#include "tools.h"
#include "Model_Simbody.h"

namespace scone
{
	namespace sim
	{
		Body_Simbody::Body_Simbody( class Model_Simbody& model, OpenSim::Body& body ) :
		Body(),
		m_osBody( body ),
		m_Model( model )
		{
			m_ForceIndex = m_osBody.getModel().getForceSet().getIndex( m_osBody.getName(), 0 );
		}

		const String& Body_Simbody::GetName() const
		{
			return m_osBody.getName();
		}

		scone::Vec3 scone::sim::Body_Simbody::GetPos() const
		{
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

			// TODO: OSIM: find what is the most efficient (compare to linvel)
			SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
			SimTK::Vec3 point;
			m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, zero, point );
			return ToVec3( point );
		}

		scone::Quat scone::sim::Body_Simbody::GetOri() const
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetLinVel() const
		{
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

			// TODO: OSIM: see if we can do this more efficient
			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.getBodyOriginVelocity( m_Model.GetTkState() ) );
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetAngVel() const
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		Vec3 Body_Simbody::GetContactForce() const
		{
			if ( m_ForceIndex != -1 )
			{
				// TODO: find out if this can be done less clumsy in OpenSim
				m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
				OpenSim::Array<double> force = m_osBody.getModel().getForceSet().get( m_ForceIndex ).getRecordValues( m_Model.GetTkState() );

				// assume total force is the first 3 values
				return Vec3( -force[0], -force[1], -force[2] );
			}
			else return Vec3::ZERO;
		}

		Vec3 Body_Simbody::GetContactTorque() const
		{
			throw std::logic_error("The method or operation is not implemented.");
		}
	}
}
