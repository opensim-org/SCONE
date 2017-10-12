#include "InterNeuron.h"

#include "scone/model/Locality.h"
#include "spot/par_tools.h"
#include "NeuralController.h"
#include "../model/Model.h"
#include "../model/Actuator.h"
#include "../model/Side.h"
#include "flut/dictionary.hpp"
#include "scone/model/Muscle.h"

namespace scone
{
	flut::dictionary< InterNeuron::connection_t > connection_dict( {
		{ InterNeuron::bilateral, "bilateral" },
		{ InterNeuron::monosynaptic, "monosynaptic" },
		{ InterNeuron::antagonistic, "antagonistic" },
		{ InterNeuron::ipsilateral, "ipsilateral" },
		{ InterNeuron::contralateral, "contralateral" } } );

	InterNeuron::InterNeuron( const PropNode& pn, Params& par, const string& name, const string& act_func ) :
	Neuron( pn, par, act_func ),
	name_( name )
	{
		side_ = GetSideFromName( name );
		INIT_PAR_NAMED( pn, par, offset_, "C0", 0 );
		INIT_PAR( pn, par, width_, 0.0 );
		use_distance_ = act_func == "gaussian"; // TODO: neater
	}

	double InterNeuron::GetOutput() const
	{
		activation_t value = offset_;
		if ( use_distance_ )
		{
			for ( auto& i : inputs_ )
				value += i.weight * flut::math::squared( i.neuron->GetOutput() - i.mean );
			value = sqrt( value );
			return output_ = gaussian_width( value, width_ );
		}
		else
		{
			for ( auto& i : inputs_ )
				value += i.weight * i.neuron->GetOutput();
		}

		return output_ = activation_function( value );
	}

	scone::string InterNeuron::GetName( bool mirrored ) const
	{
		return mirrored ? GetMirroredName( name_ ) : name_;
	}

	void InterNeuron::AddInputs( const PropNode& pn, Params& par, NeuralController& nc )
	{
		string input_type = pn.get< string >( "type", "*" );
		string input_layer = pn.get< string >( "input_layer" );
		size_t layer_size = nc.GetLayerSize( input_layer );
		connection_t connect = connection_dict( pn.get< string >( "connect", "bilateral" ) );
		bool right_side = GetSide() == RightSide;

		if ( input_layer == "0" )
		{
			for ( Index idx = 0; idx < layer_size; ++idx )
			{
				auto sensor = nc.GetSensorNeurons()[ idx ].get();
				if ( flut::pattern_match( sensor->type_, input_type ) )
				{
					switch ( connect )
					{
					case scone::InterNeuron::bilateral:
					{
						auto gain = par.try_get( sensor->GetName( right_side ), pn, "gain", 1.0 );
						auto mean = par.try_get( sensor->GetName( right_side ) + ".m", pn, "mean", 0.0 );
						AddInput( sensor, gain, mean );
						break;
					}
					case scone::InterNeuron::monosynaptic:
					{
						if ( sensor->source_name_ == name_ )
							AddInput( sensor, par.try_get( sensor->type_, pn, "gain", 1.0 ) );
						break;
					}
					case scone::InterNeuron::antagonistic:
					{
						auto& muscles = nc.GetModel().GetMuscles();
						auto it1 = TryFindByName( muscles, name_ );
						auto it2 = TryFindByName( muscles, sensor->source_name_ );
						if ( it1 != muscles.end() && it2 != muscles.end() && ( **it1 ).IsAntagonist( **it2 ) )
							AddInput( sensor, par.try_get( sensor->GetParName(), pn, "gain", 1.0 ) );
						break;
					}
					case scone::InterNeuron::ipsilateral:
					{
						if ( sensor->GetSide() == GetSide() || sensor->GetSide() == NoSide )
						{
							auto gain = par.try_get( sensor->GetParName(), pn, "gain", 1.0 );
							auto mean = par.try_get( sensor->GetParName() + ".m", pn, "mean", 0.0 );
							AddInput( sensor, gain, mean );
						}
						break;
					}
					case scone::InterNeuron::contralateral:
					{
						if ( sensor->GetSide() != GetSide() || sensor->GetSide() == NoSide )
						{
							auto gain = par.try_get( sensor->GetParName(), pn, "gain", 1.0 );
							auto mean = par.try_get( sensor->GetParName() + ".m", pn, "mean", 0.0 );
							AddInput( sensor, gain, mean );
						}
						break;
					}
					default: SCONE_THROW( "Unknown connection type " + connect );
					}
				}
			}
		}
		else
		{
			for ( Index idx = 0; idx < layer_size; ++idx )
			{
				auto input = nc.GetNeuron( input_layer, idx );
				switch ( connect )
				{
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
				default:
					break;
				}
			}
		}
	}
}
