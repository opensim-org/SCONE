#include "stdafx.h"
#include "Reflex.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Actuator.h"
#include "../sim/Area.h"
#include "Tools.h"

namespace scone
{
	namespace cs
	{
		//Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model ) :
		//m_DelayedSource( model.AcquireSensorDelayAdapter( *FindNamed( model.GetSensors(), props.GetStr( "source" ) ) ) ),
		//m_Target( *FindNamed( model.GetActuators(), props.GetStr( "target" ) ) ),
		//m_SourceIdx( props.Get< Index >( "source_index" ) )
		//{
		//	INIT_PROPERTY_REQUIRED( props, delay );
		//	INIT_FROM_PROP_PAR( props, par, gain, 0.0 );
		//	INIT_FROM_PROP_PAR( props, par, threshold, 0.0 );
		//}

		Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		m_Target( *FindByName( model.GetActuators(), props.GetStr( "target" ) + GetSideName( area.side ) ) )
		{
			INIT_PROPERTY_REQUIRED( props, delay );
		}

		Reflex::~Reflex()
		{
		}

		void Reflex::ComputeControls( double timestamp )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		scone::String Reflex::GetReflexName( const String& target, const String& source )
		{
			return ( target == source ) ? GetNameNoSide( target ) : GetNameNoSide( target ) + "-" + GetNameNoSide( source );
		}
	}
}
