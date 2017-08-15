#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "Neuron.h"
#include "flut/system/types.hpp"

namespace scone
{
	SCONE_DECLARE_STRUCT_AND_PTR( Neuron );

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		Neuron* FindInput( const PropNode& pn, Locality loc );

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) override;

		void AddMotorNeuron( Neuron* neuron, Actuator* act );
		Neuron* AddSensorNeuron( const PropNode& pn, Params& par, Model& model, Locality loc );

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< NeuronUP > m_Neurons;
		std::vector< MotorNeuron > m_MotorNeurons;
	};
}
