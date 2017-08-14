#include "NeuralController.h"

#include "flut/string_tools.hpp"
#include "scone/model/Locality.h"
#include "scone/core/HasName.h"

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

		return Controller::SuccessfulUpdate;
	}

	scone::activation_t* NeuralController::AcquireInput( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		string type = pn.get< string >( "type" );
		if ( pn.get_any< bool >( { "mirrored", "opposite" }, false ) )
			loc = MakeMirrored( loc );
		auto source = loc.ConvertName( pn.get< string >( "source" ) );

		if ( type == "Neuron" )
		{
			auto& n = FindByName( m_Neurons, source );
			return &n->output_;
		}
		else
		{
			m_SensorNeurons.push_back( std::make_unique< SensorNeuron >( pn, par, model, loc ) );
			return &m_SensorNeurons.back()->output_;
		}
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags )
	{
		for ( auto& n : m_Neurons )
			frame[ "neuron." + n->GetName() ] = n->output_;
		for ( auto& n : m_SensorNeurons )
			frame[ "neuron." + n->GetName() ] = n->output_;
	}

	String NeuralController::GetClassSignature() const
	{
		return flut::stringf( "N%d", m_Neurons.size() );
	}
}
