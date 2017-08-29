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

	struct Neuron
	{
		Neuron( const string& name = "" ) : name_( name ), output_() {}
		virtual ~Neuron() {}
		virtual activation_t GetOutput() const = 0;

		double ActivationFunction( double input ) const { return std::max( 0.0, input ); }
		const String& GetName() { return name_; }
		String name_;
		String par_name_;
		mutable double output_;
	};

	struct InterNeuron : public Neuron
	{
		InterNeuron( const PropNode& pn, Params& par, Model& model, NeuralController& controller, const Locality& locality );
		double GetOutput() const override;
		std::vector< std::pair< double, Neuron* > > inputs_;
		double offset_;
	};

	struct SensorNeuron : public Neuron
	{
		SensorNeuron( const PropNode& pn, Params& par, Model& model, Locality locality );
		double GetOutput() const override;
		SensorDelayAdapter* input_;
		TimeInSeconds delay_;
		double offset_;
		double sensor_gain_;
	};

	struct MotorNeuron
	{
		MotorNeuron( Neuron* neuron, Actuator* act ) : input_( neuron ), output_( act ) {}
		void UpdateActuator();
		Neuron* input_;
		Actuator* output_;
	};
}
