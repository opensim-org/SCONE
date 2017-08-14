#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "../core/HasData.h"

namespace scone
{
	class NeuralController;
	class SensorDelayAdapter;
	using activation_t = double;

	class Neuron_
	{
	public:
		Neuron_( const string& name = "" ) : name_( name ) {}
		virtual ~Neuron_() {}
		const String& GetName() const { return name_; }
		virtual activation_t GetOutput() const = 0;

	protected:
		String name_;
	};

	struct Neuron : public Neuron_
	{
		Neuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& locality );
		double GetOutput() const override;

		std::vector< std::pair< double, Neuron_* > > inputs_;
		mutable double output_;
		double offset_;
	};

	struct SensorNeuron : public Neuron_
	{
		SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality locality );
		double GetOutput() const override;

		mutable double output_;
		double offset_;
		SensorDelayAdapter* input_;
		TimeInSeconds delay_;
	};

	struct MotorNeuron
	{
		MotorNeuron( Neuron_* neuron, Actuator* act ) : input_( neuron ), output_( act ) {}
		void UpdateActuator();
		Neuron_* input_;
		Actuator* output_;
	};
}
