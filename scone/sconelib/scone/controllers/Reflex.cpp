#include "Reflex.h"
#include "scone/core/propnode_tools.h"
#include "scone/model/Actuator.h"
#include "scone/model/Locality.h"
#include "scone/core/math.h"

namespace scone
{
	Reflex::Reflex( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	m_Target( *FindByName( model.GetActuators(), area.ConvertName( props.get< String >( "target" ) ) ) )
	{
		INIT_PARAM_REQUIRED( props, par, delay );
		INIT_PROPERTY( props, min_control_value, REAL_LOWEST );
		INIT_PROPERTY( props, max_control_value, REAL_MAX );
	}

	Reflex::Reflex( Actuator& target, TimeInSeconds d, Real min_val, Real max_val ) :
	m_Target( target ),
	delay( d ),
	min_control_value( min_val ),
	max_control_value( max_val )
	{}

	Reflex::~Reflex() {}

	void Reflex::ComputeControls( double timestamp )
	{
		SCONE_THROW_NOT_IMPLEMENTED;
	}

	scone::Real Reflex::AddTargetControlValue( Real u )
	{
		xo::clamp( u, min_control_value, max_control_value );
		m_Target.AddInput( u );
		return u;
	}

	scone::String Reflex::GetReflexName( const String& target, const String& source )
	{
		return ( target == source ) ? target : target + "-" + source;
	}

	String Reflex::GetParName( const PropNode& props )
	{
		auto trg_name = props.get< String >( "target" );
		auto src_name = props.get< String >( "source", trg_name );
		return ( trg_name == src_name ) ? trg_name : trg_name + "-" + src_name;
	}

}
