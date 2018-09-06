#include "opt_tools.h"
#include "scone/core/types.h"
#include "scone/core/Factories.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/core/Profiler.h"

#include "xo/time/timer.h"
#include "xo/container/prop_node_tools.h"
#include "xo/filesystem/filesystem.h"

using xo::timer;

namespace scone
{
	SCONE_API OptimizerUP PrepareOptimization( const PropNode& scenario_pn, const path& scenario_file )
	{
		// create optimizer and report unused parameters
		xo::current_path( scenario_file.parent_path() ); // external resources are copied from current path
		OptimizerUP o = CreateOptimizer( scenario_pn.get_child( "Optimizer" ) );
		xo::log_unaccessed( scenario_pn );

		// return created optimizer
		return std::move( o );
	}

	PropNode SCONE_API EvaluateScenario( const PropNode& scenario_pn, const path& par_file, const path& output_base, double data_resolution )
	{
		current_path( par_file.parent_path() );

		const PropNode& objProp = scenario_pn[ "Optimizer" ][ "Objective" ];
		ObjectiveUP obj = CreateObjective( objProp );
		SimulationObjective& so = dynamic_cast<SimulationObjective&>( *obj );

		// report unused parameters
		xo::log_unaccessed( objProp );

		// set data storage
		auto model = so.CreateModelFromParFile( par_file );
		model->SetStoreData( true, data_resolution );
		Profiler::GetGlobalInstance().Reset();

		timer tmr;
		double result = so.EvaluateModel( *model );
		auto duration = tmr.seconds();

		// collect statistics
		PropNode statistics;
		statistics.set( "result", so.GetReport( *model ) );
		statistics.set( "simulation time", model->GetTime() );
		statistics.set( "performance (x real-time)", model->GetTime() / duration );

		log::info( statistics );

		// write results
		obj->WriteResults( output_base.string() );

		return statistics;
	}

	SCONE_API path FindScenario( const path& scenario_or_par_file )
	{
		if ( scenario_or_par_file.extension() == "par" )
		{
			auto folder = scenario_or_par_file.parent_path();
			return xo::find_file( { folder / "config.scone" / folder / "config.xml" } );
		}
		else return scenario_or_par_file;
	}
}
