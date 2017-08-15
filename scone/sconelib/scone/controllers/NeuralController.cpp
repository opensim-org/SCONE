#include "NeuralController.h"

#include "flut/string_tools.hpp"
#include "scone/model/Locality.h"
#include "scone/core/HasName.h"
#include "flut/container_tools.hpp"

namespace scone
{
	NeuralController::NeuralController( const PropNode& pn, Params& par, Model& model, const Locality& locality ) :
	Controller( pn, par, model, locality )
	{
		if ( auto neurons = pn.try_get_child( "Neurons" ) )
		{
			for ( auto& neuron : *neurons )
			{
				m_Neurons.push_back( std::make_unique< InterNeuron >( neuron.second, par, model, *this, Locality( LeftSide ) ) );
				m_Neurons.push_back( std::make_unique< InterNeuron >( neuron.second, par, model, *this, Locality( RightSide ) ) );
			}
		}
	}

	scone::Controller::UpdateResult NeuralController::UpdateControls( Model& model, double timestamp )
	{
		for ( auto& n : m_MotorNeurons )
			n.UpdateActuator();

		return Controller::SuccessfulUpdate;
	}

	Neuron* NeuralController::FindInput( const PropNode& pn, Locality loc )
	{
		if ( pn.get_any< bool >( { "mirrored", "opposite" }, false ) )
			loc = MakeMirrored( loc );
		auto name = loc.ConvertName( pn.get< string >( "source", "leg0" ) );


		if ( pn.get< string >( "type" ) != "Neuron" )
			name += '.' + pn.get< string >( "type" );

		auto iter = flut::find_if( m_Neurons, [&]( const NeuronUP& n ) { return n->name_ == name; } );
		return iter != m_Neurons.end() ? iter->get() : nullptr;
	}

	void NeuralController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags )
	{
		for ( auto& n : m_Neurons )
			frame[ "neuron." + n->name_ ] = n->output_;
	}

	void NeuralController::AddMotorNeuron( Neuron* neuron, Actuator* act )
	{
		m_MotorNeurons.emplace_back( neuron, act );
	}

	Neuron* NeuralController::AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc )
	{
		m_Neurons.push_back( std::make_unique< SensorNeuron >( pn, par, model, loc ) );
		return m_Neurons.back().get();
	}

	String NeuralController::GetClassSignature() const
	{
		return flut::stringf( "N%d", m_Neurons.size() );
	}
}
