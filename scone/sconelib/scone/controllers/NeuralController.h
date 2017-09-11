#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "flut/system/types.hpp"
#include "MotorNeuron.h"
#include "SensorNeuron.h"
#include "InterNeuron.h"

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

		void AddSensorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc );
		void AddInterNeuronLayer();
		void AddInterNeurons( const PropNode& pn, Params& par, Model& model, Locality loc );
		void AddMotorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc );

		Neuron* FindInput( const PropNode& pn, Locality loc );

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) override;

		std::function< double( double ) > activation_function;

	protected:
		virtual String GetClassSignature() const override;

	private:
		double std_;
		PropNode delays_;
		std::vector< std::vector< NeuronUP > > m_Neurons;
		std::vector< MotorNeuronUP > m_MotorNeurons;
	};
}
