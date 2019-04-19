/*
** optimization_test.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "scone/optimization/opt_tools.h"
#include "scone/optimization/Objective.h"

#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "xo/filesystem/path.h"

#include "xo/diagnose/test_framework.h"
#include "xo/filesystem/filesystem.h"

using namespace scone;

XO_TEST_CASE( optimization_test )
{
	xo::current_path( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "resources/unittestdata/optimization_test" );
	const PropNode pn = load_file( "schwefel_5.xml" );
	OptimizerUP o = CreateOptimizer( pn );
	xo::log_unaccessed( pn );
	o->Run();

	XO_CHECK_MESSAGE( o->GetBestFitness() < 1000.0, to_str( o->GetBestFitness() ) );
}
