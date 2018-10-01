/*
** SensorNeuron.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SensorNeuron.h"
#include "spot/par_tools.h"
#include "scone/model/Location.h"
#include "scone/model/Model.h"
#include "scone/model/Sensors.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/string_tools.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "NeuralController.h"
#include "xo/utility/hash.h"

namespace scone
{
	SensorNeuron::SensorNeuron( const PropNode& pn, Params& par, NeuralController& nc, const String& name, index_t idx, Side side, const String& act_func ) :
	Neuron( pn, name, idx, side, act_func ),
	input_sensor_(),
	sensor_gain_( 1.0 ),
	type_( pn.get< string >( "type" ) ),
	sample_delay_frames_( 0 ),
	sample_delay_window_( 21 ),
	use_sample_delay_( false )
	{
		ScopedParamSetPrefixer sp( par, GetParName() );
		delay_ = pn.get< double >( "delay", nc.GetDelay( GetNameNoSide( name ) ) );
		sample_delay_frames_ = std::lround( delay_ / nc.GetModel().GetSimulationStepSize() );
		offset_ = par.try_get( "0", pn, "offset", type_ == "L" ? 1 : 0 );
		Dof* dof = nullptr;

		auto& model = nc.GetModel();

		switch( xo::hash( type_ ) )
		{
		case "F"_hash:
			muscle_ = FindByName( model.GetMuscles(), name ).get();
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< MuscleForceSensor >( *muscle_ );
			break;
		case "L"_hash:
			muscle_ = FindByName( model.GetMuscles(), name ).get();
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< MuscleLengthSensor >( *muscle_ );
			break;
		case "S"_hash:
			muscle_ = FindByName( model.GetMuscles(), name ).get();
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< MuscleSpindleSensor >( *muscle_ );
			use_sample_delay_ = true;
			break;
		case "U"_hash:
			muscle_ = FindByName( model.GetMuscles(), name ).get();
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< MuscleExcitationSensor >( *muscle_ );
			break;
		case "DP"_hash:
			dof = FindByName( model.GetDofs(), name ).get();
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< DofPositionSensor >( *dof );
			break;
		case "DV"_hash:
			dof = FindByName( model.GetDofs(), name ).get();
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< DofVelocitySensor >( *dof );
			break;
		case "DPV"_hash:
			dof = FindByName( model.GetDofs(), name ).get();
			auto kv = pn.has_key( "velocity_gain" ) ?
				par.try_get( ".DV", pn, "velocity_gain", 0.1 ) :
				par.get( ".DV", 0.1, 0.01, 0, 1 ); // this is for backwards compatibility (<1845)
			auto parent_name = pn.try_get_any< string >( { "parent", "source_parent" } );
			Dof* root_dof = parent_name ? FindByName( model.GetDofs(), *parent_name ).get() : nullptr;
			input_sensor_ = &nc.GetModel().AcquireDelayedSensor< DofPosVelSensor >( *dof, kv, root_dof );
			break;
		}

		// mirror sensor gain for right side sensors of MirrorDofs
		if ( dof && nc.IsMirrorDof( *dof ) && side == RightSide )
			sensor_gain_ *= -1;

		xo_error_if( !input_sensor_, "Unknown type " + type_ );
		source_name_ = name;
	}

	double SensorNeuron::GetOutput( double offset ) const
	{
		auto input = use_sample_delay_ ? input_sensor_->GetAverageValue( sample_delay_frames_, sample_delay_window_ ) : input_sensor_->GetValue( delay_ );
		return output_ = activation_function( sensor_gain_ * ( input - offset_ - offset ) );
	}

	string SensorNeuron::GetName( bool mirrored ) const
	{
		if ( !mirrored )
			return source_name_ + "." + type_;
		else return GetMirroredName( source_name_ ) + "." + type_;
	}

	string SensorNeuron::GetParName() const
	{
		return Neuron::GetParName() + '.' + type_;
	}
}
