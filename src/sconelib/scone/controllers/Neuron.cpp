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

namespace scone
{
	xo::dictionary< InterNeuron::connection_t > connection_dict( {
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
			double gain = 0;
			auto mpvec = nc.GetMuscleParams( muscle_, false );
			auto spvec = nc.GetMuscleParams( sensor->muscle_, true );

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
						auto par_value = par.try_get( parname, pn, "gain", 0.0 );
						gain += factor * par_value;
						//log::trace( "\t", parname, " = ", factor, " x ", par_value );
					}
				}
			}
			//log::trace( "\tTOTAL = ", gain );

			AddInput( sensor, gain );
		}
	}

	void Neuron::AddInputs( const PropNode& pn, Params& par, NeuralController& nc )
	{
		SCONE_PROFILE_FUNCTION;

		// set param prefix
		auto mpars = nc.GetMuscleParams( muscle_, false );
		if ( pn.has_key( "offset" ) )
		{
			for ( auto& mp : mpars )
			{
				//log::trace( muscle_->GetName(), "\t", mp.first, " x ", mp.second );
				offset_ += mp.correlation * par.try_get( mp.name + ".C0", pn, "offset", 0.0 );
			}
		}

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
				if ( xo::pattern_match( sensor->type_, input_type ) )
				{
					string postfix;
					switch ( connect )
					{
					case InterNeuron::bilateral:
					{
						postfix = sensor->GetName( right_side );
						//AddInput( sensor, par.try_get( sensor->GetName( right_side ), pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::monosynaptic:
					{
						if ( sensor->source_name_ == name_ )
							postfix = sensor->type_;
						//AddInput( sensor, par.try_get( sensor->type_, pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::antagonistic:
					{
						if ( muscle_ && sensor->muscle_ && muscle_->IsAntagonist( *sensor->muscle_ ) )
							postfix = sensor->GetParName();
						//AddInput( sensor, par.try_get( sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::agonistic:
					{
						if ( muscle_ && sensor->muscle_ && muscle_->IsAgonist( *sensor->muscle_ ) )
							postfix = sensor->muscle_ == muscle_ ? sensor->type_ : sensor->GetParName();
						//AddInput( sensor, par.try_get( sensor->muscle_ == muscle_ ? sensor->type_ : sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					case InterNeuron::synergetic:
					{
						if ( muscle_ && sensor->muscle_ )
							AddSynergeticInput( sensor, pn, par, nc );
						break;
					}
					case InterNeuron::ipsilateral:
					{
						if ( sensor->GetSide() == GetSide() || sensor->GetSide() == NoSide )
							postfix = sensor->GetParName();
						break;
					}
					case InterNeuron::contralateral:
					{
						if ( sensor->GetSide() != GetSide() || sensor->GetSide() == NoSide )
							postfix = sensor->GetParName();
						//AddInput( sensor, par.try_get( sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					default: SCONE_THROW( "Invalid connection type: " + connection_dict( connect ) );
					}

					if ( !postfix.empty() )
					{
						double gain = 0, offset = 0;
						for ( auto& mp : mpars )
						{
							gain += mp.correlation * par.try_get( mp.name + '.' + postfix, pn, "gain", 1.0 );
							offset += mp.correlation * par.try_get( mp.name + '.' + postfix + '0', pn, "offset", 0.0 );
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
