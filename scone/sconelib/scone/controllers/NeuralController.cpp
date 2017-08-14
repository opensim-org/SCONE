#include "NeuralController.h"

#include "flut/string_tools.hpp"
#include "scone/model/Locality.h"
#include "../core/HasName.h"

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality )
	{
		if ( auto neurons = pn.try_get_child( "Neurons" ) )
		{
			for ( auto& neuron : *neurons )
			{
				m_Neurons.push_back( std::make_unique< Neuron >( neuron.second, par, model, *this, Locality( LeftSide ) ) );
				m_Neurons.push_back( std::make_unique< Neuron >( neuron.second, par, model, *this, Locality( RightSide ) ) );
			}
		}
	}

	scone::Controller::UpdateResult NeuralController::UpdateControls( Model& model, double timestamp )
	{
		for ( auto& n : m_SensorNeurons )
			n->UpdateOutput();

		for ( auto& n : m_Neurons )
			n->UpdateOutput();

		for ( auto& n : m_MotorNeurons )
			n->UpdateActuator();

		return Controller::SuccessfulUpdate;
	}

	scone::activation_t* NeuralController::AcquireInput( const PropNode& pn, Params& par, Model& model, const Locality& loc )
	{
		string type = pn.get< string >( "type" );
		if ( type == "Neuron" )
		{
			auto& n = FindByName( m_Neurons, pn.get< string >( "source" ) );
			return &n->output_;
		}
		else
		{
			m_SensorNeurons.push_back( std::make_unique< SensorNeuron >( pn, par, model, loc ) );
			return &m_SensorNeurons.back()->output_;
		}
	}

	String NeuralController::GetClassSignature() const
	{
		return flut::stringf( "N%d", m_Neurons.size() );
	}
}
