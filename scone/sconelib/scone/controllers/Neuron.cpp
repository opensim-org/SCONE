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
		// remove existing muscle name prefix (TODO: more elegant)
		auto prefix = par.pop_prefix();

		// add joint names to par prefix
		string joint_prefix;
		for ( const Link* l = &muscle_->GetInsertionLink(); l != &muscle_->GetOriginLink(); l = &l->GetParent() )
			joint_prefix += GetNameNoSide( l->GetJoint().GetName() ) + ".";
		joint_prefix[ joint_prefix.size() - 1 ] = '-';
		for ( const Link* l = &sensor->muscle_->GetInsertionLink(); l != &sensor->muscle_->GetOriginLink(); l = &l->GetParent() )
			joint_prefix += GetNameNoSide( l->GetJoint().GetName() ) + ".";
		joint_prefix[ joint_prefix.size() - 1 ] = '-';

		double gain = 0;
		for ( auto& dof : muscle_->GetModel().GetDofs() )
		{
			auto muscle_mom = muscle_->GetNormalizedMomentArm( *dof );
			auto sensor_mom = sensor->muscle_->GetNormalizedMomentArm( *dof );
			if ( muscle_mom != 0 && sensor_mom != 0 )
			{
				string dof_name = GetNameNoSide( dof->GetName() ) + '.' + SignChar( muscle_mom ) + SignChar( sensor_mom );
				auto factor = sqrt( abs( muscle_mom * sensor_mom ) );
				gain += par.try_get( joint_prefix + dof_name, pn, "gain", 0.0 ) * factor;
			}
		}
		if ( gain != 0 )
			AddInput( sensor, gain );

		par.push_prefix( prefix );
	}

	void Neuron::AddInputs( const PropNode& pn, Params& par, NeuralController& nc )
	{
		SCONE_PROFILE_FUNCTION;

		// set param prefix
		ScopedParamSetPrefixer ps( par, GetParName() + "." );

		// add additional input-specific offset (if present)
		offset_ += par.try_get( "C0", pn, "offset", 0.0 );

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
							AddInput( sensor, par.try_get( sensor->GetParName(), pn, "gain", 1.0 ) );
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
