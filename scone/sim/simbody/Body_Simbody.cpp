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
		}

		const String& Body_Simbody::GetName() const
		{
			return m_osBody.getName();
		}

		scone::Vec3 scone::sim::Body_Simbody::GetPos()
		{
			//m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );
			//const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			//return ToVec3( mob.getBodyOriginLocation( m_Model.GetTkState() ) );
			SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
			SimTK::Vec3 point;
			m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, zero, point );
			return ToVec3( point );
		}
		
		scone::Quat scone::sim::Body_Simbody::GetOri()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetLinVel()
		{
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );
			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.getBodyOriginVelocity( m_Model.GetTkState() ) );
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetAngVel()
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}
	}
}
