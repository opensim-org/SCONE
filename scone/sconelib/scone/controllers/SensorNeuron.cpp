#include "SensorNeuron.h"
#include "spot/par_tools.h"
#include "../model/Locality.h"
#include "../model/Model.h"
#include "../model/Sensors.h"
#include "../model/SensorDelayAdapter.h"
#include "scone/core/string_tools.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "NeuralController.h"

namespace scone
{
	SensorNeuron::SensorNeuron( NeuralController& nc, const PropNode& pn, Params& par, Model& model, Locality loc ) :
	Neuron( nc ),
	input_(),
	offset_(),
	sensor_gain_( 1.0 ),
	type_( pn.get< string >( "type" ) )
	{
		bool opposite = pn.get_any< bool >( { "mirrored", "opposite" }, false );
		bool inverted = pn.get< bool >( "inverted", false );
		const auto source_name = pn.get< string >( "source", "leg" );
		par_name_ = source_name + ( opposite ? "_o." : "." ) + type_;
		ScopedParamSetPrefixer sp( par, par_name_ );
		INIT_PROP_REQUIRED( pn, delay_ );
		INIT_PAR( pn, par, offset_, type_ == "L" ? 1 : ( inverted ? 1 : 0 ) );
		INIT_PROP( pn, sensor_gain_, inverted ? -1 : 1 );

		if ( opposite )
			loc = MakeMirrored( loc );

		SetInputSensor( model, type_, source_name, loc );
	}

	SensorNeuron::SensorNeuron( NeuralController& nc, Model& model, const string& type, const string& source, double delay, double offset, bool inverted ) :
	Neuron( nc ),
	offset_( offset ),
	delay_( delay ),
	sensor_gain_( inverted ? -1 : 1 ),
	type_( type )
	{
		SetInputSensor( model, type_, source, Locality( NoSide ) );
		par_name_ = source_name_ + "." + type_;
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
		else if ( type == "S" )
		{
			input_ = &model.AcquireDelayedSensor< MuscleSpindleSensor >( *FindByName( model.GetMuscles(), loc.ConvertName( name ) ) );
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
		source_name_ = name;
	}

	double SensorNeuron::GetOutput() const
	{
		return output_ = controller_.sensor_activation_function( sensor_gain_ * ( input_->GetValue( delay_ ) - offset_ ) );
	}

	scone::string SensorNeuron::GetName( bool mirrored ) const
	{
		if ( !mirrored )
			return par_name_;
		else return GetMirroredName( source_name_ ) + "." + type_;
	}
}
