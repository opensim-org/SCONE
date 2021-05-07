/*
** opt_tools.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "scone/core/PropNode.h"
#include "scone/core/string_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Log.h"
#include "scone/core/types.h"
#include "scone/optimization/Optimizer.h"

namespace scone
{
	/// Log unused properties
	SCONE_API bool LogUnusedProperties( const PropNode& pn );

	/// Creates and evaluates SimulationObjective. Logs unused properties.
	SCONE_API PropNode EvaluateScenario( const PropNode& scenario_pn, const path& par_file, const path& output_base );

	/// Returns .scone file for a given .par file, or returns argument if already .scone.
	SCONE_API path FindScenario( const path& scenario_or_par_file );
}
