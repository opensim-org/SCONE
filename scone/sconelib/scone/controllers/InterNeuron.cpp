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
		{ InterNeuron::universal, "universal" },
		{ InterNeuron::monosynaptic, "monosynaptic" },
		{ InterNeuron::antagonistic, "antagonistic" },
		{ InterNeuron::ipsilateral, "ipsilateral" },
		{ InterNeuron::contralateral, "contralateral" } } );

	InterNeuron::InterNeuron( const PropNode& pn, Params& par, NeuralController& nc, const string& name ) :
	Neuron( pn, par, nc ),
	name_( name )
	{
		side_ = GetSideFromName( name );
		INIT_PAR_NAMED( pn, par, offset_, "C0", 0 );
		INIT_PAR( pn, par, width_, 0.0 );
		use_distance_ = pn.get< string >( "activation", "" ) == "gaussian"; // TODO: neater
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
		Index input_layer = pn.get< Index >( "input_layer" );
		connection_t connect = connection_dict( pn.get< string >( "connect", "universal" ) );
		bool right_side = GetSide() == RightSide;

		// add inputs
		if ( input_layer == 0 )
		{
			for ( Index idx = 0; idx < nc.GetSensorNeurons().size(); ++idx )
			{
				auto sensor = nc.GetSensorNeurons()[ idx ].get();
				if ( flut::pattern_match( sensor->type_, input_type ) )
				{
					if ( connect == antagonistic )
					{
						auto it1 = TryFindByName( nc.GetModel().GetMuscles(), name_ );
						auto it2 = TryFindByName( nc.GetModel().GetMuscles(), sensor->source_name_ );
						if ( it1 != nc.GetModel().GetMuscles().end() && it2 != nc.GetModel().GetMuscles().end() )
						{
							if ( ( *it1 )->IsAntagonist( **it2 ) )
								AddInput( sensor, par.get( sensor->GetParName(), pn[ "gain" ] ) );
						}
					}
					else if ( connect == monosynaptic && sensor->source_name_ == name_ )
						AddInput( sensor, par.get( sensor->type_, pn[ "gain" ] ) );
					else if ( connect == ipsilateral && sensor->GetSide() == GetSide() )
						AddInput( sensor, par.get( sensor->GetParName(), pn[ "gain" ] ), par.try_get( sensor->GetParName() + ".m", pn, "mean", 0.0 ) );
					else if ( connect == contralateral && sensor->GetSide() != GetSide() )
						AddInput( sensor, par.get( sensor->GetParName(), pn[ "gain" ] ), par.try_get( sensor->GetParName() + ".m", pn, "mean", 0.0 ) );
					else if ( connect == universal )
						AddInput( sensor, par.get( sensor->GetName( right_side ), pn[ "gain" ] ), par.try_get( GetName( right_side ) + ".m", pn, "mean", 0.0 ) );
				}
			}
		}
		else
		{
			for ( Index idx = 0; idx < nc.GetLayerSize( input_layer ); ++idx )
			{
				// TODO: move this to upper code
				auto input = nc.GetNeuron( input_layer, idx );

				if ( connect == ipsilateral && input->GetSide() == GetSide() )
					AddInput( input, par.get( input->GetParName(), pn[ "gain" ] ) );
				else if ( connect == contralateral && input->GetSide() != GetSide() )
					AddInput( input, par.get( input->GetParName(), pn[ "gain" ] ) );
				else if ( connect == universal )
					AddInput( input, par.get( input->GetName( right_side ), pn[ "gain" ] ) );
			}
		}
	}
}
