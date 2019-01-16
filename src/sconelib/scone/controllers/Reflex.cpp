/*
** Reflex.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Reflex.h"
#include "scone/model/Actuator.h"
#include "scone/model/Location.h"
#include "scone/core/math.h"

namespace scone
{
	Reflex::Reflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	target( props.get< String >( "target" ) ),
	actuator_( *FindByLocation( model.GetActuators(), target, loc ) )
	{
		INIT_PAR( props, par, delay, 0 );
		INIT_PROP( props, min_control_value, xo::constants<Real>::lowest() );
		INIT_PROP( props, max_control_value, xo::constants<Real>::max() );
	}

	Reflex::Reflex( Actuator& target, TimeInSeconds d, Real min_val, Real max_val ) :
	actuator_( target ),
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
		actuator_.AddInput( u );
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
