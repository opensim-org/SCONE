#include "Reflex.h"
#include "scone/core/propnode_tools.h"
#include "scone/sim/Actuator.h"
#include "scone/sim/Area.h"

namespace scone
{
	namespace cs
	{
		Reflex::Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		m_Target( *FindByName( model.GetActuators(), props.GetStr( "target" ) + GetSideName( area.side ) ) )
		{
			INIT_PROPERTY_REQUIRED( props, delay );
			INIT_PROPERTY( props, min_control_value, REAL_LOWEST );
			INIT_PROPERTY( props, max_control_value, REAL_MAX );
		}

		Reflex::~Reflex() {}

		void Reflex::ComputeControls( double timestamp )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

		scone::Real Reflex::AddTargetControlValue( Real u )
		{
			flut::math::clamp( u, min_control_value, max_control_value );
			m_Target.AddControlValue( u );
			return u;
		}

		scone::String Reflex::GetReflexName( const String& target, const String& source )
		{
			return ( target == source ) ? GetNameNoSide( target ) : GetNameNoSide( target ) + "-" + GetNameNoSide( source );
		}
	}
}
