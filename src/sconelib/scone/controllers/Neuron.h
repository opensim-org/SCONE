#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/HasData.h"
#include "activation_functions.h"
#include "../model/Side.h"

namespace scone
{
	class NeuralController;
	struct SensorNeuron;
	using activation_t = double;

	struct Neuron
	{
		Neuron( const PropNode& pn, Index idx, Side s, const String& act_func );
		virtual ~Neuron() {}
		virtual activation_t GetOutput() const;
		virtual string GetName( bool mirrored = false ) const { return mirrored ? GetMirroredName( name_ ) : name_; }
		virtual string GetParName() const { return GetNameNoSide( name_ ); }
		Side GetSide( bool mirrored = false ) { return mirrored ? GetMirroredSide( side_ ) : side_; }

		enum connection_t { bilateral, monosynaptic, antagonistic, agonistic, synergetic, ipsilateral, contralateral };
		void AddInput( Neuron* input, double gain, double mean = 0.0 ) { inputs_.emplace_back( input, gain, mean ); }
		void AddSynergeticInput( SensorNeuron* sensor, const PropNode& pn, Params& par, NeuralController& nc );
		void AddInputs( const PropNode& pn, Params& par, NeuralController& nc );

		struct Input {
			Input( Neuron* n, double g, double c ) : neuron( n ), gain( g ), offset( c ), contribution( 0 ) {}
			Neuron* neuron;
			double gain;
			double offset;
			mutable double contribution = 0.0;
		};
		const std::vector< Input >& GetInputs() const { return inputs_; }

		Side side_;
		Index index_;
		string name_;
		Muscle* muscle_;

		double offset_;
		mutable double output_;
		mutable double input_;
		activation_func_t activation_function;

		std::vector< Input > inputs_;
	};
}
