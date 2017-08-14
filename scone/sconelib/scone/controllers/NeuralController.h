#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/model/Controller.h"
#include "Neuron.h"
#include "flut/system/types.hpp"

namespace scone
{
	SCONE_DECLARE_STRUCT_AND_PTR( Neuron );
	SCONE_DECLARE_STRUCT_AND_PTR( SensorNeuron );
	SCONE_DECLARE_STRUCT_AND_PTR( MotorNeuron );

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}


		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		activation_t* AcquireInput( const PropNode& pn, Params& par, Model& model, const Locality& loc );

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< SensorNeuronUP > m_SensorNeurons;
		std::vector< NeuronUP > m_Neurons;
		std::vector< MotorNeuronUP > m_MotorNeurons;
	};
}
