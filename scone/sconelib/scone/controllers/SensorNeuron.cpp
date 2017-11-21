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
	SensorNeuron::SensorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const String& name, Index idx, Side side, const String& act_func ) :
	Neuron( pn, idx, side, act_func ),
	input_(),
	sensor_gain_( 1.0 ),
	type_( pn.get< string >( "type" ) ),
	sample_delay_frames_( 0 ),
	sample_delay_window_( 21 ),
	use_sample_delay_( false )
	{
		bool inverted = pn.get< bool >( "inverted", false );
		par_name_ = GetNameNoSide( name ) + ( inverted ? "-." : "." ) + type_;
		side_ = GetSideFromName( name );

		ScopedParamSetPrefixer sp( par, par_name_ );
		delay_ = pn.get< double >( "delay", nc.GetDelay( GetNameNoSide( name ) ) );
		sample_delay_frames_ = std::lround( delay_ / nc.GetModel().GetSimulationStepSize() );
		offset_ = par.try_get( "0", pn, "offset", type_ == "L" ? 1 : ( inverted ? 1 : 0 ) );
		sensor_gain_ = inverted ? -1 : 1;

		auto& model = nc.GetModel();

		if ( type_ == "F" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< MuscleForceSensor >( *FindByName( model.GetMuscles(), name ) );
		}
		else if ( type_ == "L" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< MuscleLengthSensor >( *FindByName( model.GetMuscles(), name ) );
		}
		else if ( type_ == "S" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< MuscleSpindleSensor >( *FindByName( model.GetMuscles(), name ) );
			use_sample_delay_ = true;
		}
		else if ( type_ == "DP" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< DofPositionSensor >( *FindByName( model.GetDofs(), name ) );
		}
		else if ( type_ == "DV" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< DofVelocitySensor >( *FindByName( model.GetDofs(), name ) );
		}
		else if ( type_ == "DPV" )
		{
			auto kv = par.get( ".DV", 0.1, 0.01, 0, 1 );
			input_ = &nc.GetModel().AcquireDelayedSensor< DofPosVelSensor >( *FindByName( model.GetDofs(), name ), kv );
		}

		flut_error_if( !input_, "Unknown type " + type_ );
		source_name_ = name;
	}

	double SensorNeuron::GetOutput() const
	{
		auto input = use_sample_delay_ ? input_->GetAverageValue( sample_delay_frames_, sample_delay_window_ ) : input_->GetValue( delay_ );
		return output_ = activation_function( sensor_gain_ * ( input - offset_ ) );
	}

	string SensorNeuron::GetName( bool mirrored ) const
	{
		if ( !mirrored )
			return source_name_ + "." + type_;
		else return GetMirroredName( source_name_ ) + "." + type_;
	}

	string SensorNeuron::GetParName() const
	{
		return par_name_;
	}
}
