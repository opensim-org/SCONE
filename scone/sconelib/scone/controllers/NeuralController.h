#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "flut/system/types.hpp"
#include "MotorNeuron.h"
#include "SensorNeuron.h"
#include "InterNeuron.h"
#include "activation_functions.h"

namespace scone
{
	SCONE_DECLARE_STRUCT_AND_PTR( Neuron );
	SCONE_DECLARE_STRUCT_AND_PTR( InterNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( SensorNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( MotorNeuron );

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}

		SensorNeuron* AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc );
		InterNeuron* AddInterNeuron( const PropNode& pn, Params& par, Model& model, Locality loc );

		void AddSensorNeurons( const PropNode& pn, Params& par, Model& model );
		void AddInterNeuronLayer();
		void AddInterNeurons( const PropNode& pn, Params& par, Model& model, bool mirrored );
		void AddMotorNeurons( const PropNode& pn, Params& par, Model& model, bool mirrored );

		Neuron* FindInput( const PropNode& pn, Locality loc );
		size_t GetLayerSize( Index layer ) const { return ( layer == 0 ) ? m_SensorNeurons.size() : m_InterNeurons[ layer - 1 ].size(); }
		Neuron* GetNeuron( Index layer, Index idx ) { return ( layer == 0 ) ? dynamic_cast< Neuron* >( m_SensorNeurons[ idx ].get() ) : dynamic_cast< Neuron* >( m_InterNeurons[ layer - 1 ][ idx ].get() ); }

		Model& GetModel() { return model_; }

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

		PropNode delays_;
		activation_func_t activation_function;
		virtual void WriteResult( const path& file ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Model& model_;
		std::vector< SensorNeuronUP > m_SensorNeurons;
		std::vector< std::vector< InterNeuronUP > > m_InterNeurons;
		std::vector< MotorNeuronUP > m_MotorNeurons;
	};
}
