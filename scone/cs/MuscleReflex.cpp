#include "stdafx.h"

#include "MuscleReflex.h"
#include "../sim/Muscle.h"

//#define DEBUG_MUSCLE "hamstrings_r"

namespace scone
{
	namespace cs
	{
		MuscleReflex::MuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Actuator& target, sim::Sensor& source ) :
		Reflex( props, par, model, target, source )
		{
			INIT_PARAM_NAMED( props, par, length_gain, "KL", 0.0 );
			INIT_PARAM_NAMED( props, par, length_ofs, "L0", 1.0 );
			INIT_PARAM_NAMED( props, par, velocity_gain, "KV", 0.0 );
			INIT_PARAM_NAMED( props, par, force_gain, "KF", 0.0 );
		}

		MuscleReflex::~MuscleReflex()
		{
		}

		void MuscleReflex::ComputeControls( double timestamp )
		{
			// add stretch reflex
			Real u_l = length_gain * std::max( 0.0, m_DelayedSource.GetSensorValue( sim::Muscle::MuscleLengthSensor, delay ) - length_ofs );

			// add velocity reflex
			// TODO: should velocity gain be positive only?
			Real u_v = velocity_gain * std::max( 0.0, m_DelayedSource.GetSensorValue( sim::Muscle::MuscleVelocitySensor, delay ) );

			// add force reflex
			Real u_f = force_gain * m_DelayedSource.GetSensorValue( sim::Muscle::MuscleForceSensor, delay );

			m_Target.AddControlValue( u_l + u_v + u_f );

#ifdef DEBUG_MUSCLE
			if ( m_Target.GetName() == DEBUG_MUSCLE )
				log::TraceF( "u_l=%.3f u_v=%.3f u_f=%.3f", u_l, u_v, u_f );
#endif
		}
	}
}
