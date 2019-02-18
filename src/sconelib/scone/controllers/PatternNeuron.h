/*
** PatternNeuron.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Neuron.h"

namespace scone
{
	struct PatternNeuron : public Neuron
	{
	public:
		PatternNeuron( const PropNode& pn, Params& par, NeuralController& nc, int index, bool mirrored );
		virtual ~PatternNeuron() {}

		virtual activation_t GetOutput( double offset = 0.0 ) const override;
		virtual string GetName( bool mirrored ) const override { return name_ + ( mirrored_ ? "_r" : "_l" ); }

		bool mirrored_;

	private:
		double width_;
		double beta_;
		double t0_;
		double period_;
		const class Model& model_;
	};
}
