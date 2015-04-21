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
			INIT_FROM_PROP_PAR( props, par, length_gain );
			INIT_FROM_PROP_PAR( props, par, length_ofs );
			INIT_FROM_PROP_PAR( props, par, velocity_gain );
			INIT_FROM_PROP_PAR( props, par, force_gain );
		}

		Reflex::~Reflex()
		{
		}

		void Reflex::UpdateControls( double timestamp )
		{
			// update internal measures
			m_Length.Update( timestamp, m_Source.GetFiberLength() );
			m_Velocity.Update( timestamp, m_Source.GetFiberVelocity() );
			m_Force.Update( timestamp, m_Source.GetForce() );

			// compute reflex excitation u
			double u = 0.0;
			u += length_gain * std::max( 0.0, m_Length.GetDelayed() - length_ofs );
			u += velocity_gain * m_Velocity.GetDelayed();
			u += force_gain * m_Force.GetDelayed();

			// apply to target
			m_Target.AddControlValue( u );
		}
	}
}
