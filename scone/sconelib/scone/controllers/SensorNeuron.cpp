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
	Neuron( pn, par, idx, side, act_func ),
	input_(),
	sensor_gain_( 1.0 ),
	type_( pn.get< string >( "type" ) )
	{
		bool inverted = pn.get< bool >( "inverted", false );
		par_name_ = GetNameNoSide( name ) + ( inverted ? "-." : "." ) + type_;
		side_ = GetSideFromName( name );

		ScopedParamSetPrefixer sp( par, par_name_ );
		delay_ = pn.get< double >( "delay", nc.delays_.get< double >( GetNameNoSide( name ) ) );
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
		}
		else if ( type_ == "DP" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< DofPositionSensor >( *FindByName( model.GetDofs(), name ) );
		}
		else if ( type_ == "DV" )
		{
			input_ = &nc.GetModel().AcquireDelayedSensor< DofVelocitySensor >( *FindByName( model.GetDofs(), name ) );
		}

		flut_assert_msg( input_, "Unknown type " + type_ );
		source_name_ = name;
	}

	double SensorNeuron::GetOutput() const
	{
		return output_ = activation_function( sensor_gain_ * ( input_->GetValue( delay_ ) - offset_ ) );
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
