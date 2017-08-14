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

	class NeuralController : public Controller
	{
	public:
		NeuralController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~NeuralController() {}


		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		Neuron_* AcquireNeuron( const PropNode& pn, Params& par, Model& model, Locality loc );

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) override;

		void AddMotorNeuron( Neuron_* neuron, Actuator* act );
	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< SensorNeuronUP > m_SensorNeurons;
		std::vector< NeuronUP > m_Neurons;
		std::vector< MotorNeuron > m_MotorNeurons;
	};
}
