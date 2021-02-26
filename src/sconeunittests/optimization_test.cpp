/*
** optimization_test.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "scone/optimization/CmaOptimizerSpot.h"
#include "scone/optimization/Objective.h"
#include "scone/optimization/opt_tools.h"

#include "xo/filesystem/filesystem.h"
#include "xo/filesystem/path.h"
#include "xo/serialization/serialize.h"
#include "xo/system/test_case.h"

using namespace scone;

XO_TEST_CASE( optimization_test )
{
	auto test_folder = scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "resources/unittestdata/optimization_test";
	const PropNode pn = xo::load_file( test_folder / "schwefel_5.xml" );
	OptimizerUP o = CreateOptimizer( pn, test_folder );
	o->output_root = xo::temp_directory_path() / "SCONE/optimization_test";
	xo::log_unaccessed( pn );
	o->Run();

	XO_CHECK_MESSAGE( o->GetBestFitness() < 1000.0, to_str( o->GetBestFitness() ) );
}
