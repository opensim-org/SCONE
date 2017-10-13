#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "flut/system/types.hpp"
#include "MotorNeuron.h"
#include "SensorNeuron.h"
#include "InterNeuron.h"
#include "PatternNeuron.h"
#include "activation_functions.h"
#include "flut/flat_map.hpp"

namespace scone
{
	SCONE_DECLARE_STRUCT_AND_PTR( Neuron );
	SCONE_DECLARE_STRUCT_AND_PTR( InterNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( SensorNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( MotorNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( PatternNeuron );

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}

		size_t GetLayerSize( const string& layer ) const { return ( layer == "0" ) ? m_SensorNeurons.size() : m_InterNeurons[ layer ].size(); }
		Neuron* GetNeuron( const string& layer, Index idx ) { return ( layer == "0" ) ? dynamic_cast< Neuron* >( m_SensorNeurons[ idx ].get() ) : dynamic_cast< Neuron* >( m_InterNeurons[ layer ][ idx ].get() ); }

		const Model& GetModel() const { return model_; }
		Model& GetModel() { return model_; }

		std::vector< SensorNeuronUP >& GetSensorNeurons() { return m_SensorNeurons; }

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

		PropNode delays_;
		activation_func_t activation_function;
		virtual void WriteResult( const path& file ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Model& model_;

		void AddSensorNeuronLayer( const PropNode& pn, Params& par );
		void AddPatternNeurons( const PropNode& pn, Params& par );
		void AddInterNeuronLayer( const PropNode& pn, Params& par );
		void AddMotorNeuronLayer( const PropNode& pn, Params& par );

		std::vector< PatternNeuronUP > m_PatternNeurons;
		std::vector< SensorNeuronUP > m_SensorNeurons;
		flut::flat_map< string, std::vector< InterNeuronUP > > m_InterNeurons;
		std::vector< MotorNeuronUP > m_MotorNeurons;
	};
}
