#include "SimpleMuscleReflex.h"

#include "scone/core/HasName.h"
#include "../model/Model.h"
#include "../model/Actuator.h"
#include "../model/Sensors.h"
#include "../model/Muscle.h"

using namespace flut;

namespace scone
{
	SimpleMuscleReflex::SimpleMuscleReflex( const String& target, const String& source, double mean, double stdev, TimeInSeconds delay, Model& model, Params& par, const Locality& area ) :
	Reflex( *FindByName( model.GetActuators(), area.ConvertName( target ) ), delay ),
	m_Source( nullptr )
	{
		if ( !source.empty() )
		{
			auto src_name = left_of_str( source, "." );
			auto src_type = right_of_str( source, "." );
			Muscle& src_act = *FindByName( model.GetMuscles(), area.ConvertName( src_name ) );
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
			m_Target.AddInput( m_Gain * m_Source->GetValue( delay ) );
		else m_Target.AddInput( m_Gain );
	}

	void SimpleMuscleReflex::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags )
	{
	}
}
