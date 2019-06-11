/*
** SimpleMuscleReflex.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SimpleMuscleReflex.h"

#include "scone/core/HasName.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/model/Sensors.h"
#include "scone/model/Muscle.h"

using namespace xo;

namespace scone
{
	SimpleMuscleReflex::SimpleMuscleReflex( const String& target, const String& source, double mean, double stdev, TimeInSeconds delay, Model& model, Params& par, const Location& loc ) :
	Reflex( *FindByLocation( model.GetActuators(), target, loc ), delay ),
	m_Source( nullptr )
	{
		if ( !source.empty() )
		{
			auto src_name = left_of_str( source, "." );
			auto src_type = right_of_str( source, "." );
			Muscle& src_act = *FindByLocation( model.GetMuscles(), src_name, loc );
			if ( src_type == "fiber_force" )
				m_Source = &model.AcquireDelayedSensor< MuscleForceSensor >( src_act );
			else if ( src_type == "fiber_length" )
				m_Source = &model.AcquireDelayedSensor< MuscleLengthSensor >( src_act );
			else if ( src_type == "fiber_velocity" )
				m_Source = &model.AcquireDelayedSensor< MuscleVelocitySensor >( src_act );
			else SCONE_THROW( "Unknown sensor type: " + src_type );
		}
		m_Gain = par.get( target + "-" + source, mean, stdev );
	}

	void SimpleMuscleReflex::ComputeControls( double timestamp )
	{
		if ( m_Source )
			actuator_.AddInput( m_Gain * m_Source->GetValue( delay ) );
		else actuator_.AddInput( m_Gain );
	}
}
