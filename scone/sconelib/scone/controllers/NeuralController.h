#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "Neuron.h"

namespace scone
{
	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}

	private:
		std::vector< SensorNeuron > m_SensorNeurons;
		std::vector< Neuron > m_Neurons;
		std::vector< MotorNeuron > m_MotorNeurons;
	};
}
