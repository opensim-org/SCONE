/*
** Neuron.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/HasData.h"
#include "activation_functions.h"
#include "scone/model/Side.h"

namespace scone
{
	class NeuralController;
	struct SensorNeuron;
	using activation_t = double;

	struct Neuron
	{
		Neuron( const PropNode& pn, const String& name, index_t idx, Side s, const String& act_func );
		virtual ~Neuron() {}
		virtual activation_t GetOutput( double offset = 0.0 ) const;
		virtual string GetName( bool mirrored = false ) const { return mirrored ? GetMirroredName( name_ ) : name_; }
		virtual string GetParName() const;
		Side GetSide( bool mirrored = false ) { return mirrored ? GetOppositeSide( side_ ) : side_; }

		enum connection_t { none, bilateral, monosynaptic, antagonistic, agonistic, synergetic, synergetic_dof, synergetic_plus, ipsilateral, contralateral, source };
		void AddInput( Neuron* input, double gain, double offset = 0.0 ) { inputs_.emplace_back( input, gain, offset ); }
		void AddSynergeticInput( SensorNeuron* sensor, const PropNode& pn, Params& par, NeuralController& nc );
		void AddInputs( const PropNode& pn, Params& par, NeuralController& nc );

		struct Input {
			Input( Neuron* n, double g, double o ) : neuron( n ), gain( g ), offset( o ), contribution( 0 ) {}
			Neuron* neuron;
			double gain;
			double offset;
			mutable double contribution = 0.0;
		};
		const std::vector< Input >& GetInputs() const { return inputs_; }

		Side side_;
		index_t index_;
		string name_;
		Muscle* muscle_;

		double offset_;
		mutable double output_;
		mutable double input_;
		activation_func_t activation_function;
		bool symmetric_;

		std::vector< Input > inputs_;

	private:
		bool CheckRelation( connection_t connect, SensorNeuron* sensor, const PropNode& pn );
	};
}
