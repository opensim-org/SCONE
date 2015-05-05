#include "stdafx.h"
#include "Reflex.h"

namespace scone
{
	namespace cs
	{
		Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Muscle& source, sim::Muscle& target ) :
		m_Source( source ),
		m_Target( target ),
		m_Length( props.GetReal( "delay" ) ),
		m_Velocity( props.GetReal( "delay" ) ),
		m_Force( props.GetReal( "delay" ) )
		{
			INIT_FROM_PROP_PAR( props, par, length_gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, length_ofs, 1.0 );
			INIT_FROM_PROP_PAR( props, par, velocity_gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, force_gain, 0.0 );
		}

		Reflex::~Reflex()
		{
		}

		void Reflex::UpdateControls( double timestamp )
		{
			UpdateMuscleState( timestamp );
			ComputeControls( timestamp );
		}

		void Reflex::UpdateMuscleState( double timestamp )
		{
			// update internal measures
			m_Length.AddSample( timestamp, m_Source.GetNormalizedFiberLength() );
			m_Velocity.AddSample( timestamp, m_Source.GetNormalizedFiberVelocity() );
			m_Force.AddSample( timestamp, m_Source.GetNormalizedFiberForce() );
		}

		void Reflex::ComputeControls( double timestamp )
		{
			// compute reflex excitation u
			double u = 0.0;

			// add stretch reflex
			u += length_gain * std::max( 0.0, m_Length.GetDelayedValue( timestamp ) - length_ofs );

			// add velocity reflex
			// TODO: should velocity gain be positive only?
			u += velocity_gain * std::max( 0.0, m_Velocity.GetDelayedValue( timestamp ) );

			// add force reflex
			u += force_gain * m_Force.GetDelayedValue( timestamp );

			// apply excitation to target muscle
			m_Target.AddControlValue( u );
		}
	}
}
