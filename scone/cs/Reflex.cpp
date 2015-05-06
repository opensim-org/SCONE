#include "stdafx.h"
#include "Reflex.h"

namespace scone
{
	namespace cs
	{
		Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Muscle& source, sim::Muscle& target ) :
		m_Source( source ),
		m_Target( target ),
		m_DelayedSource( model.AcquireSensorDelayAdapter( source ) )
		{
			INIT_FROM_PROP_PAR( props, par, length_gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, length_ofs, 1.0 );
			INIT_FROM_PROP_PAR( props, par, velocity_gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, force_gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, delay, 0.0 );
		}

		Reflex::~Reflex()
		{
		}

		void Reflex::ComputeControls( double timestamp )
		{
			// compute reflex excitation u
			double u = 0.0;

			// add stretch reflex
			u += length_gain * std::max( 0.0, m_DelayedSource.GetSensorValue( sim::Muscle::MuscleLengthSensor, delay ) - length_ofs );

			// add velocity reflex
			// TODO: should velocity gain be positive only?
			u += velocity_gain * std::max( 0.0, m_DelayedSource.GetSensorValue( sim::Muscle::MuscleVelocitySensor, delay ) );

			// add force reflex
			u += force_gain * m_DelayedSource.GetSensorValue( sim::Muscle::MuscleForceSensor, delay );

			// apply excitation to target muscle
			m_Target.AddControlValue( u );
		}
	}
}
