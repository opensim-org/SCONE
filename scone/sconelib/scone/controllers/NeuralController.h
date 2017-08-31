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
	SCONE_DECLARE_STRUCT_AND_PTR( InterNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( SensorNeuron );

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}

		SensorNeuron* AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc );
		InterNeuron* AddInterNeuron( const PropNode& pn, Params& par, Model& model, Locality loc );
		void AddMotorNeuron( Neuron* neuron, Actuator* act );

		void AddSensorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc );
		void AddInterNeurons( const PropNode& pn, Params& par, Model& model, Locality loc );
		void AddMotorNeurons( const PropNode& pn, Params& par, Model& model, Locality loc );

		Neuron* FindInput( const PropNode& pn, Locality loc );

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< SensorNeuronUP > m_SensorNeurons;
		std::vector< InterNeuronUP > m_InterNeurons;
		std::vector< InterNeuronUP > m_MotorInterNeurons;
		std::vector< MotorNeuron > m_MotorNeurons;
	};
}
