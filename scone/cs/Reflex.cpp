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
