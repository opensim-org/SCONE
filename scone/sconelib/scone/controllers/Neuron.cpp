#include "Neuron.h"

#include "activation_functions.h"
#include "NeuralController.h"
#include "spot/par_tools.h"
#include "flut/dictionary.hpp"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Actuator.h"
#include "scone/model/Side.h"
#include "scone/model/Dof.h"
#include "../core/Profiler.h"
#include "../model/Link.h"
#include "../model/Joint.h"
#include "../model/model_tools.h"

namespace scone
{
	flut::dictionary< InterNeuron::connection_t > connection_dict( {
		{ Neuron::bilateral, "bilateral" },
		{ Neuron::monosynaptic, "monosynaptic" },
		{ Neuron::antagonistic, "antagonistic" },
		{ Neuron::agonistic, "agonistic" },
		{ Neuron::synergetic, "synergetic" },
		{ Neuron::ipsilateral, "ipsilateral" },
		{ Neuron::contralateral, "contralateral" },
		{ Neuron::agonistic, "protagonistic" } // backwards compatibility
	} );

	Neuron::Neuron( const PropNode& pn, Index idx, Side s, const String& default_activation ) :
	output_(),
	offset_(),
	index_( idx ),
	side_( s ),
	activation_function( GetActivationFunction( pn.get< string >( "activation", default_activation ) ) ),
	muscle_( nullptr )
	{}

	scone::activation_t Neuron::GetOutput() const
	{
		activation_t value = offset_;
		for ( auto& i : inputs_ )
		{
			auto input = i.gain * i.neuron->GetOutput();
			i.contribution += abs( input );
			value += input;
		}

		return output_ = activation_function( value );
	}

	void Neuron::AddSynergeticInput( SensorNeuron* sensor, const PropNode& pn, Params& par, NeuralController& nc )
	{
		auto& mjoints = muscle_->GetJoints();
		auto& sjoints = sensor->muscle_->GetJoints();

		// check if muscle and input have common joints
		int common_joints = 0;
		for ( auto& mj : mjoints )
			for ( auto& sj : sjoints )
				common_joints += ( mj == sj );

		if ( common_joints > 0 )
		{
			//auto prefix = par.pop_prefix();

			double gain = 0;
			auto mvmvec = nc.GetMuscleParams( *muscle_ );
			auto svmvec = nc.GetMuscleParams( *sensor->muscle_ );

			for ( auto& mvm : mvmvec )
			{
				for ( auto& svm : svmvec )
				{
					string parname = ( mvm.first == svm.first ? mvm.first : mvm.first + '.' + svm.first ) + '.' + sensor->type_;
					auto factor = sqrt( mvm.second * svm.second );
					gain += factor * par.try_get( parname, pn, "gain", 0.0 );
				}
			}

			if ( gain != 0 )
				AddInput( sensor, gain );

			//par.push_prefix( prefix );
		}
	}

	void Neuron::AddInputs( const PropNode& pn, Params& par, NeuralController& nc )
	{
		SCONE_PROFILE_FUNCTION;

		// set param prefix
		auto mpars = nc.GetMuscleParams( *muscle_ );

		if ( pn.has_key( "offset" ) )
		{
			for ( auto& mp : mpars )
			{
				log::trace( muscle_->GetName(), "\t", mp.first, " x ", mp.second );
				offset_ += mp.second * par.try_get( mp.first + ".C0", pn, "offset", 0.0 );
			}
		}

#if 0
		ScopedParamSetPrefixer ps( par, par_name + "." );
		// add additional input-specific offset (if present)
		offset_ += par.try_get( "C0", pn, "offset", 0.0 );
#endif

		// see if there's an input
		string input_type = pn.get< string >( "type", "*" );
		string input_layer = NeuralController::FixLayerName( pn.get< string >( "input_layer", "" ) );

		connection_t connect = connection_dict( pn.get< string >( "connect", "bilateral" ) );
		bool right_side = GetSide() == RightSide;

		if ( input_layer == "0" )
		{
			// connection from sensor neurons
			size_t input_layer_size = nc.GetLayerSize( input_layer );
			for ( Index idx = 0; idx < input_layer_size; ++idx )
			{
				auto sensor = nc.GetSensorNeurons()[ idx ].get();
				if ( flut::pattern_match( sensor->type_, input_type ) )
				{
					switch ( connect )
					{
					case InterNeuron::bilateral:
					{
						AddInput( sensor, par.try_get( sensor->GetName( right_side ), pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::monosynaptic:
					{
						SCONE_PROFILE_SCOPE( "monosynaptic" );
						if ( sensor->source_name_ == name_ )
							AddInput( sensor, par.try_get( sensor->type_, pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::antagonistic:
					{
						SCONE_PROFILE_SCOPE( "antagonistic" );
						if ( muscle_ && sensor->muscle_ && muscle_->IsAntagonist( *sensor->muscle_ ) )
							AddInput( sensor, par.try_get( sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::agonistic:
					{
						SCONE_PROFILE_SCOPE( "agonistic" );
						if ( muscle_ && sensor->muscle_ && muscle_->IsAgonist( *sensor->muscle_ ) )
							AddInput( sensor, par.try_get( sensor->muscle_ == muscle_ ? sensor->type_ : sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::synergetic:
					{
						SCONE_PROFILE_SCOPE( "synergetic" );
						if ( muscle_ && sensor->muscle_ )
							AddSynergeticInput( sensor, pn, par, nc );
						break;
					}
					case InterNeuron::ipsilateral:
					{
						if ( sensor->GetSide() == GetSide() || sensor->GetSide() == NoSide )
						{
							// TODO: neater!
							double gain = 0;
							for ( auto& mp : mpars )
								gain += mp.second * par.try_get( mp.first + '.' + sensor->type_, pn, "gain", 1.0 );
							AddInput( sensor, gain );
						}
						break;
					}
					case InterNeuron::contralateral:
					{
						if ( sensor->GetSide() != GetSide() || sensor->GetSide() == NoSide )
							AddInput( sensor, par.try_get( sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					default: SCONE_THROW( "Invalid connection type: " + connection_dict( connect ) );
					}
				}
			}
		}
		else if ( !input_layer.empty() )
		{
			// connection from previous interneuron layer
			size_t input_layer_size = nc.GetLayerSize( input_layer );
			for ( Index idx = 0; idx < input_layer_size; ++idx )
			{
				auto input = nc.GetNeuron( input_layer, idx );
				switch ( connect )
				{
				case scone::InterNeuron::monosynaptic:
					if ( input->index_ == index_ )
						AddInput( input, par.try_get( input->GetParName(), pn, "gain", 1.0 ) );
					break;
				case scone::InterNeuron::bilateral:
					AddInput( input, par.try_get( input->GetName( right_side ), pn, "gain", 1.0 ) );
					break;
				case scone::InterNeuron::ipsilateral:
					if ( input->GetSide() == GetSide() || input->GetSide() == NoSide )
						AddInput( input, par.try_get( input->GetParName(), pn, "gain", 1.0 ) );
					break;
				case scone::InterNeuron::contralateral:
					if ( input->GetSide() != GetSide() || input->GetSide() == NoSide )
						AddInput( input, par.try_get( input->GetParName(), pn, "gain", 1.0 ) );
					break;
				default: SCONE_THROW( "Invalid connection type: " + connection_dict( connect ) );
				}
			}
		}
	}
}
