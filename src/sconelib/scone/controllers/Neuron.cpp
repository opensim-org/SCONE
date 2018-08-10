#include "Neuron.h"

#include "activation_functions.h"
#include "NeuralController.h"
#include "spot/par_tools.h"
#include "xo/string/dictionary.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/model/Actuator.h"
#include "scone/model/Side.h"
#include "scone/model/Dof.h"
#include "../core/Profiler.h"
#include "../model/Link.h"
#include "../model/Joint.h"
#include "../model/model_tools.h"
#include "xo/container/container_tools.h"

namespace scone
{
	xo::dictionary< InterNeuron::connection_t > connection_dict( {
		{ Neuron::none, "none" },
		{ Neuron::bilateral, "bilateral" },
		{ Neuron::monosynaptic, "monosynaptic" },
		{ Neuron::antagonistic, "antagonistic" },
		{ Neuron::agonistic, "agonistic" },
		{ Neuron::synergetic, "synergetic" },
		{ Neuron::synergetic_dof, "synergetic_dof" },
		{ Neuron::synergetic_plus, "synergetic_plus" },
		{ Neuron::ipsilateral, "ipsilateral" },
		{ Neuron::contralateral, "contralateral" },
		{ Neuron::source, "source" },
		{ Neuron::agonistic, "protagonistic" } // backwards compatibility
	} );

	Neuron::Neuron( const PropNode& pn, index_t idx, Side s, const String& default_activation ) :
	output_(),
	offset_(),
	index_( idx ),
	side_( s ),
	activation_function( GetActivationFunction( pn.get< string >( "activation", default_activation ) ) ),
	muscle_( nullptr ),
	input_()
	{
		INIT_PROP( pn, symmetric_, true );
	}

	scone::activation_t Neuron::GetOutput( double offset ) const
	{
		activation_t value = offset_ + offset;
		for ( auto& i : inputs_ )
		{
			auto input = i.gain * i.neuron->GetOutput( i.offset );
			i.contribution += abs( input );
			value += input;
		}
		input_ = value;
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
			double gain = 0, offset = 0;
			auto mpvec = nc.GetMuscleParams( muscle_, false, GetSide() == RightSide );
			auto spvec = nc.GetMuscleParams( sensor->muscle_, true, GetSide() == RightSide );

			//log::trace( muscle_->GetName(), " <-- ", sensor->GetParName() );
			for ( auto& mp : mpvec )
			{
				for ( auto& sp : spvec )
				{
					if ( std::find_first_of( mp.dofs.begin(), mp.dofs.end(), sp.dofs.begin(), sp.dofs.end(), 
						[&]( Dof* a, Dof* b ) { return a != b && &a->GetJoint() == &b->GetJoint(); } ) == mp.dofs.end() )
					{
						string parname = ( mp.name == sp.name ? mp.name : mp.name + '.' + sp.name ) + '.' + sensor->type_;
						auto factor = mp.correlation * sp.correlation;
						gain += factor * par.try_get( parname, pn, "gain", 0.0 );
						offset += factor * par.try_get( parname + '0', pn, "offset", 0.0 );
						//log::trace( "\t", parname, "; factor=", factor, " gain=", gain, " offset=", offset );
					}
				}
			}
			//log::trace( "\tTOTAL gain=", gain, " offset=", offset );

			AddInput( sensor, gain, offset );
		}
	}

	bool Neuron::CheckRelation( connection_t connect, SensorNeuron* sensor, const PropNode& pn )
	{
		switch ( connect )
		{
		case InterNeuron::bilateral: return true;
		case InterNeuron::monosynaptic: return muscle_ && sensor->source_name_ == name_;
		case InterNeuron::antagonistic: return muscle_ && sensor->muscle_ && muscle_->IsAntagonist( *sensor->muscle_ );
		case InterNeuron::agonistic: return muscle_ && sensor->muscle_ && muscle_->IsAgonist( *sensor->muscle_ );
		case InterNeuron::synergetic:
		case InterNeuron::synergetic_dof:
			return muscle_ && sensor->muscle_ && muscle_->HasSharedDofs( *sensor->muscle_ );
		case InterNeuron::synergetic_plus:
			return muscle_ && sensor->muscle_ && muscle_->GetSide() == sensor->muscle_->GetSide()
				&& ( muscle_->HasSharedBodies( *sensor->muscle_ ) || muscle_->HasSharedDofs( *sensor->muscle_ ) );
		case InterNeuron::ipsilateral: return sensor->GetSide() == GetSide() || sensor->GetSide() == NoSide;
		case InterNeuron::contralateral: return sensor->GetSide() != GetSide() || sensor->GetSide() == NoSide;
		case InterNeuron::source:
			return GetNameNoSide( sensor->source_name_ ) == pn.get< string >( "source" )
				&& ( muscle_->GetSide() == sensor->GetSide() || sensor->GetSide() == NoSide );
		case InterNeuron::none: return false;
		default: SCONE_THROW( "Invalid connection type: " + connection_dict( connect ) );
		}
	}

	bool CheckMuscleParamRelation( Neuron::connection_t connect, const NeuralController::MuscleParam& mp, const NeuralController::MuscleParam& sp )
	{
		switch ( connect )
		{
		case scone::Neuron::monosynaptic:
		case scone::Neuron::antagonistic:
		case scone::Neuron::agonistic:
		case scone::Neuron::synergetic_dof:
			// checks if mp and sp have at least one dof in common
			// this is done to make sure only relevant virtual muscle parameters are used
			// not sure if this is the best way to do it, though.
			return 0 < xo::count_if( mp.dofs, [&]( auto& e ) { return xo::find( sp.dofs, e ) != sp.dofs.end(); } );
		default:
			return true;
		}
	}

	void Neuron::AddInputs( const PropNode& pn, Params& par, NeuralController& nc )
	{
		SCONE_PROFILE_FUNCTION;

		// see if there's an input
		connection_t connect = connection_dict( pn.get< string >( "connect", pn.has_key( "source" ) ? "source" : "none" ) );
		string input_type = pn.get< string >( "type", "*" );
		string input_layer = NeuralController::FixLayerName( pn.get< string >( "input_layer", connect == none ? "" : "0" ) );
		bool right_side = GetSide() == RightSide;

		auto mpvec = nc.GetMuscleParams( muscle_, false, right_side );

		// check the input layer
		if ( input_layer == "0" )
		{
			// connection from sensor neurons
			size_t input_layer_size = nc.GetLayerSize( input_layer );
			for ( index_t idx = 0; idx < input_layer_size; ++idx )
			{
				auto sensor = nc.GetSensorNeurons()[ idx ].get();
				if ( xo::pattern_match( sensor->type_, input_type ) )
				{
					if ( CheckRelation( connect, sensor, pn ) )
					{
						double gain = 0, offset = 0;
						if ( sensor->muscle_ )
						{
							// input sensor is a muscle, so we need to find all muscle params
							auto spvec = nc.GetMuscleParams( sensor->muscle_, true, right_side );
							for ( auto& mp : mpvec )
							{
								for ( auto& sp : spvec )
								{
									string parname = ( mp.name == sp.name ? mp.name : mp.name + '.' + sp.name ) + '.' + sensor->type_;
									auto factor = mp.correlation * sp.correlation;
									// TODO: this whole relation thing should be reconsidered
									bool relation_ok = nc.GetParMode() == NeuralController::muscle_mode || CheckMuscleParamRelation( connect, mp, sp );
									if ( relation_ok && factor >= nc.min_virtual_muscle_correlation )
									{
										gain += factor * par.try_get( parname, pn, "gain", 0.0 );
										offset += factor * par.try_get( parname + '0', pn, "offset", 0.0 );
									}
								}
							}
						}
						else
						{
							// input sensor is no muscle, so we need no breakdown (TODO: a little neater)
							for ( auto& mp : mpvec )
							{
								gain += mp.correlation * par.try_get( mp.name + '.' + sensor->GetParName(), pn, "gain", 1.0 );
								offset += mp.correlation * par.try_get( mp.name + '.' + sensor->GetParName() + '0', pn, "offset", 0.0 );
							}
						}

						AddInput( sensor, gain );
					}
				}
			}
		}
		else if ( !input_layer.empty() )
		{
			// connection from previous interneuron layer
			size_t input_layer_size = nc.GetLayerSize( input_layer );
			for ( index_t idx = 0; idx < input_layer_size; ++idx )
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
		else if ( pn.has_key( "offset" ) )
		{
			// this is a channel with only an offset -- used for backwards compatibility
			for ( auto& mp : mpvec )
				offset_ += mp.correlation * par.try_get( mp.name + ".C0", pn, "offset", 0.0 );
		}
	}
}
