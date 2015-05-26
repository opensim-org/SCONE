#include "stdafx.h"
#include "Reflex.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Actuator.h"

namespace scone
{
	namespace cs
	{
		Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		m_DelayedSource( model.AcquireSensorDelayAdapter( *FindNamed( model.GetSensors(), props.GetStr( "source" ) ) ) ),
		m_Target( *FindNamed( model.GetActuators(), props.GetStr( "target" ) ) ),
		m_SourceIdx( props.Get< Index >( "source_index" ) )
		{
			INIT_PROPERTY_REQUIRED( props, delay );
			INIT_FROM_PROP_PAR( props, par, gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, offset, 0.0 );
		}

		Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, sim::Sensor& source, Index source_idx, sim::Actuator& target ) :
		m_DelayedSource( model.AcquireSensorDelayAdapter( source ) ),
		m_Target( target ),
		m_SourceIdx( source_idx )
		{
			INIT_PROPERTY_REQUIRED( props, delay );
			INIT_FROM_PROP_PAR( props, par, gain, 0.0 );
			INIT_FROM_PROP_PAR( props, par, offset, 0.0 );
		}

		Reflex::~Reflex()
		{
		}

		void Reflex::ComputeControls( double timestamp )
		{
			Real v = m_DelayedSource.GetSensorValue( m_SourceIdx, delay );
			if ( v > offset )
				m_Target.AddControlValue( gain * ( v - offset ) );
		}
	}
}
