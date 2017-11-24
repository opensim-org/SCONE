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
	using activation_t = double;

	struct Neuron
	{
		Neuron( const PropNode& pn, Index idx, Side s, const String& act_func );
		virtual ~Neuron() {}
		virtual activation_t GetOutput() const;
		virtual string GetName( bool mirrored ) const { return mirrored ? GetMirroredName( name_ ) : name_; }
		virtual string GetParName() const { return GetNameNoSide( name_ ); }
		Side GetSide( bool mirrored = false ) { return mirrored ? GetMirroredSide( side_ ) : side_; }

		enum connection_t { bilateral, monosynaptic, antagonistic, protagonistic, ipsilateral, contralateral };

		void AddInput( Neuron* input, double gain, double mean = 0.0 ) { inputs_.emplace_back( input, gain, mean ); }
		void AddInputs( const PropNode& pn, Params& par, NeuralController& nc );
		size_t GetInputCount() { return inputs_.size(); }

		Side side_;
		Index index_;
		string name_;
		Muscle* muscle_;

		double offset_;
		mutable double output_;
		activation_func_t activation_function;

		struct Input {
			Input( Neuron* n, double g, double c ) : neuron( n ), gain( g ), center( c ), contribution( 0 ) {}
			Neuron* neuron;
			double gain;
			double center;
			mutable double contribution = 0.0;
		};

		std::vector< Input > inputs_;
	};
}
