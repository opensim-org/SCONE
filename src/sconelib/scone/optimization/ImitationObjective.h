/*
** ImitationObjective.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
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
#include "scone/core/Storage.h"
#include "ModelObjective.h"

namespace scone
{
	/// Objective that attempts to imitate the results of an existing simulation, without having to perform a new a simulation.
	class SCONE_API ImitationObjective : public ModelObjective
	{
	public:
		ImitationObjective( const PropNode& props, const path& find_file_folder );
		virtual ~ImitationObjective();

		/// File containing the existing simulation results (.sto).
		path file;

		/// Number of frames to skip during each evaluation step; default = 1.
		size_t frame_delta;

		virtual void AdvanceSimulationTo( Model& m, TimeInSeconds t ) const override;
		virtual TimeInSeconds GetDuration() const override;
		virtual fitness_t GetResult( Model& m ) const override;
		virtual PropNode GetReport( Model& m ) const override;

	private:
		Storage<> m_Storage;
		std::vector< index_t > m_ExcitationChannels;
		std::vector< index_t > m_SensorChannels;
	};
}
