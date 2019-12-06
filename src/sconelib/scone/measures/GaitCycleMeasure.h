/*
** GaitCycleMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "scone/model/State.h"

namespace scone
{
	/// Measure how well a single gait cycle is performed.
	/// *work in progress*, for gait optimization, use GaitMeasure instead.
	class GaitCycleMeasure : public Measure
	{
	public:
		GaitCycleMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		virtual double ComputeResult( const Model& model ) override;

		/// Use half gait cycle instead of full cycle; default = false.
		bool use_half_cycle;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override { return false; }

	private:
		Real GetStateSimilarity( const State& state );
		std::pair< String, int > GetMirroredStateNameAndSign( const String& str );

		State m_InitState;
	};
}
