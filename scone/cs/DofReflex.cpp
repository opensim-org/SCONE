#include "stdafx.h"

#include "DofReflex.h"
#include "../sim/Dof.h"
#include "../sim/Actuator.h"

namespace scone
{
	namespace cs
	{
		DofReflex::DofReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::Sensor& source ) :
		Reflex( props, par, model, target, source )
		{
			SCONE_ASSERT( dynamic_cast< sim::Dof* >( &source ) != 0 );
			INIT_FROM_PROP_PAR( props, par, target_pos, 0.0 );
			INIT_FROM_PROP_PAR( props, par, target_vel, 0.0 );
			INIT_FROM_PROP_PAR( props, par, pos_gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, vel_gain, 0.0 );
		}

		DofReflex::~DofReflex()
		{
		}

		void DofReflex::ComputeControls( double timestamp )
		{
			Real u_p = pos_gain * ( target_pos - GetDelayedSensorValue( sim::Dof::DofPositionSensor ) );
			Real u_d = vel_gain * ( target_vel - GetDelayedSensorValue( sim::Dof::DofVelocitySensor ) );
			m_Target.AddControlValue( u_p + u_d );
		}
	}
}
