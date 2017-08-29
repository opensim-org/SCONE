#include "SensorNeuron.h"
#include "spot/par_tools.h"
#include "../model/Locality.h"
#include "../model/Model.h"
#include "../model/Sensors.h"
#include "../model/SensorDelayAdapter.h"
#include "scone/core/string_tools.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"

namespace scone
{
	SensorNeuron::SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc ) :
	Neuron( "" ),
	input_(),
	offset_(),
	sensor_gain_( 1.0 )
	{
		const auto type = pn.get< string >( "type" );
		bool opposite = pn.get_any< bool >( { "mirrored", "opposite" }, false );
		bool inverted = pn.get< bool >( "inverted", false );
		const auto source_name = pn.get< string >( "source", "leg" );
		par_name_ = source_name + ( opposite ? "_o." : "." ) + type;
		ScopedParamSetPrefixer sp( par, par_name_ );

		INIT_PROP_REQUIRED( pn, delay_ );
		INIT_PAR( pn, par, offset_, type == "L" ? 1 : ( inverted ? 1 : 0 ) );
		INIT_PROP( pn, sensor_gain_, inverted ? -1 : 1 );

		if ( opposite )
			loc = MakeMirrored( loc );

		SetInputSensor( model, type, source_name, loc );
	}

	SensorNeuron::SensorNeuron( SensorDelayAdapter* input, double delay, double offset, bool inverted ) :
		delay_( delay ),
		input_( input ),
		offset_( offset ),
		sensor_gain_( inverted ? -1 : 1 )
	{
		name_ = input_->GetName() + stringf( "+%.0f", delay_ * 1000 );
	}

	double SensorNeuron::GetOutput() const
	{
		return output_ = ActivationFunction( sensor_gain_ * ( input_->GetValue( delay_ ) - offset_ ) );
	}

	void SensorNeuron::SetInputSensor( Model& model, const string& type, const string& name, const Locality& loc )
	{
		if ( type == "F" )
		{
			input_ = &model.AcquireDelayedSensor< MuscleForceSensor >( *FindByName( model.GetMuscles(), loc.ConvertName( name ) ) );
		}
		else if ( type == "L" )
		{
			input_ = &model.AcquireDelayedSensor< MuscleLengthSensor >( *FindByName( model.GetMuscles(), loc.ConvertName( name ) ) );
		}
		else if ( type == "DP" )
		{
			input_ = &model.AcquireDelayedSensor< DofPositionSensor >( *FindByName( model.GetDofs(), name ) );
		}
		else if ( type == "DV" )
		{
			input_ = &model.AcquireDelayedSensor< DofVelocitySensor >( *FindByName( model.GetDofs(), name ) );
		}
		else if ( type == "LD" )
		{
			input_ = &model.AcquireDelayedSensor< LegLoadSensor >( model.GetLeg( loc ) );
		}

		flut_assert_msg( input_, "Unknown type " + type );
		name_ = input_->GetName() + stringf( "+%.0f", delay_ * 1000 );
	}
}
