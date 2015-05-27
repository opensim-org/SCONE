#include "stdafx.h"

#include "DofReflex.h"
#include "../sim/Dof.h"
#include "../sim/Actuator.h"

//#define DEBUG_MUSCLE "glut_max_r"

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
			Real pos = GetDelayedSensorValue( sim::Dof::DofPositionSensor );
			Real vel = GetDelayedSensorValue( sim::Dof::DofVelocitySensor );
			Real u_p = pos_gain * ( target_pos - pos );
			Real u_d = vel_gain * ( target_vel - vel );
			m_Target.AddControlValue( u_p + u_d );

#ifdef DEBUG_MUSCLE
			if ( m_Target.GetName() == DEBUG_MUSCLE )
				log::TraceF( "pos=%.3f vel=%.3f u_p=%.3f u_d=%.3f", pos, vel, u_p, u_d );
#endif
		}
	}
}
