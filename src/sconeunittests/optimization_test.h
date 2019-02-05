#include "scone/optimization/opt_tools.h"
#include "scone/optimization/Objective.h"

#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "xo/filesystem/path.h"

#include "xo/diagnose/test_framework.h"
#include "xo/filesystem/filesystem.h"

using namespace scone;

void optimization_test()
{
	xo::current_path( scone::GetFolder( scone::SCONE_ROOT_FOLDER ) / "unittestdata/optimization_test" );
	const PropNode pn = load_file( "schwefel_5.xml" );
	OptimizerUP o = CreateOptimizer( pn.get_child( "Optimizer" ) );
	xo::log_unaccessed( pn );
	o->Run();

	XO_TEST_MSG( o->GetBestFitness() < 1000.0, to_str( o->GetBestFitness() ) );
}
