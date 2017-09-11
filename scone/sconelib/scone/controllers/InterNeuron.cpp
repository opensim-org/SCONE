#include "InterNeuron.h"

#include "scone/model/Locality.h"
#include "spot/par_tools.h"
#include "NeuralController.h"
#include "../model/Model.h"
#include "../model/Actuator.h"

namespace scone
{
	InterNeuron::InterNeuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& loc ) :
	Neuron( controller ),
	name_( loc.ConvertName( pn.get< string >( "name" ) ) )
	{
		auto par_name = GetNameNoSide( name_ ) + '.';
		ScopedParamSetPrefixer sp( par, par_name );
		INIT_PAR( pn, par, offset_, 0 );

		for ( auto& input_pn : pn )
		{
			if ( input_pn.first == "Input" )
			{
				Neuron* input = controller.FindInput( input_pn.second, loc );
				if ( !input )
					input = controller.AddSensorNeuron( input_pn.second, par, model, loc );

				double gain = par.get( GetNameNoSide( input->GetName( false ) ), input_pn.second[ "gain" ] );
				inputs_.push_back( std::make_pair( gain, input ) );
				//log::info( name_, " <-- ", gain, " * ", input->GetName() );
			}
		}

		// set target actuator (if any)
		// TODO: move to MotorNeuron
		//if ( pn.has_key( "target" ) )
		//	controller.AddMotorNeuron( this, FindByName( model.GetActuators(), loc.ConvertName( pn.get< string >( "target" ) ) ) );
	}

	double InterNeuron::GetOutput() const
	{
		activation_t value = offset_;
		for ( auto& i : inputs_ )
			value += i.first * i.second->GetOutput();

		return output_ = controller_.activation_function( value );
	}

	scone::string InterNeuron::GetName( bool mirrored ) const
	{
		return mirrored ? GetMirroredName( name_ ) : name_;
	}
}
