/*
** SimulationObjective.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/optimization/Objective.h"
#include "scone/optimization/Params.h"
#include "scone/model/Simulation.h"
#include "scone/core/PropNode.h"

#include "scone/measures/Measure.h"
#include <vector>
#include "xo/filesystem/path.h"
#include "ModelObjective.h"

namespace scone
{
	/// Objective in which a simulation is run.
	class SCONE_API SimulationObjective : public ModelObjective
	{
	public:
		SimulationObjective( const PropNode& props );
		virtual ~SimulationObjective();

		/// Maximum duration after which the evaluation is terminated; default = 1e12 (+/-31000 years)
		double max_duration;

		virtual fitness_t EvaluateModel( Model& m ) const override;
		virtual TimeInSeconds GetDuration() const override { return max_duration; }

		virtual void AdvanceSimulationTo( Model& m, TimeInSeconds t ) const override;
		virtual fitness_t GetResult( Model& m ) const override { return m.GetMeasure()->GetWeightedResult( m ); }
		virtual PropNode GetReport( Model& m ) const override { return m.GetMeasure()->GetReport(); }
	};
}
