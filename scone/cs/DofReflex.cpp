#include "stdafx.h"

#include "DofReflex.h"
#include "../sim/Dof.h"
#include "../sim/Actuator.h"
#include "../sim/Model.h"
#include "../sim/SensorDelayAdapter.h"

//#define DEBUG_MUSCLE "glut_max_r"

namespace scone
{
	namespace cs
	{
		DofReflex::DofReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Reflex( props, par, model, area ),
		m_DelayedPos( model.AcquireSensorDelayAdapter( FindByName( model.GetDofs(), props.GetStr( "source" ) )->m_PositionSensor ) ),
		m_DelayedVel( model.AcquireSensorDelayAdapter( FindByName( model.GetDofs(), props.GetStr( "source" ) )->m_VelocitySensor ) ),
		m_DelayedRootPos( model.AcquireSensorDelayAdapter( FindByName( model.GetDofs(), "pelvis_tilt" )->m_PositionSensor ) ),
		m_DelayedRootVel( model.AcquireSensorDelayAdapter( FindByName( model.GetDofs(), "pelvis_tilt" )->m_VelocitySensor ) )
		//m_DelayedRoot( model.AcquireSensorDelayAdapter( *FindByName( model.GetSensors(), "pelvis_tilt" ) ) ),
		//m_bUseRoot( m_DelayedRoot.GetName() != m_DelayedSource.GetName() )
		{
			// TODO: don't use this hack
			//SCONE_ASSERT( dynamic_cast< sim::Dof* >( &source ) != 0 );
			INIT_PARAM_NAMED( props, par, target_pos, "P0", 0.0 );
			INIT_PARAM_NAMED( props, par, target_vel, "V0", 0.0 );
			INIT_PARAM_NAMED( props, par, pos_gain, "KP", 0.0 );
			INIT_PARAM_NAMED( props, par, vel_gain, "KV", 0.0 );
		}

		DofReflex::~DofReflex()
		{
		}

		void DofReflex::ComputeControls( double timestamp )
		{
			// TODO: Add world coordinate option to Body
			Real root_pos = m_DelayedRootPos.GetValue( delay );
			Real root_vel = m_DelayedRootVel.GetValue( delay );
			Real pos = m_DelayedPos.GetValue( delay );
			Real vel = m_DelayedVel.GetValue( delay );

			if ( m_bUseRoot )
			{
				pos += root_pos;
				vel += root_vel;
			}

			Real u_p = pos_gain * ( target_pos - pos );
			Real u_d = vel_gain * ( target_vel - vel );
			m_Target.AddControlValue( u_p + u_d );

#ifdef DEBUG_MUSCLE
			if ( m_Target.GetName() == DEBUG_MUSCLE )
				log::TraceF( "pos=%.3f vel=%.3f root_pos=%.3f root_vel=%.3f u_p=%.3f u_d=%.3f", pos, vel, root_pos, root_vel, u_p, u_d );
#endif
		}
	}
}
