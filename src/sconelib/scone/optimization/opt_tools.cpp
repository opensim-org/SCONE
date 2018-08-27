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
	SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const path& scenario_file )
	{
		// set current path to scenario file folder
		xo::current_path( scenario_file.parent_path() );

		// create optimizer and report unused parameters
		OptimizerUP o = CreateOptimizer( props.get_child( "Optimizer" ) );
		LogUntouched( props );

		// copy original and write resolved config files
		xo::path outdir( o->AcquireOutputFolder() );
		xo::copy_file( scenario_file.filename(), outdir / path( "config_original" ).replace_extension( scenario_file.extension() ), true );
		xo::save_file( props, outdir / "config.scone" );
		xo::copy_file( scenario_file.parent_path() / o->init_file, outdir / o->init_file.filename(), true );

		// copy all objective resources to output folder
		for ( auto& f : o->GetObjective().GetExternalResources() )
			xo::copy_file( f, outdir / f.filename(), true );

		// set current path to output folder
		xo::current_path( outdir );

		// return created optimizer
		return std::move( o );
	}

	PropNode SCONE_API SimulateObjective( const path& filename )
	{
		auto folder = filename.parent_path();
		auto config_path = xo::find_file( { folder / "config.xml", folder / "config.scone" } );
		if ( config_path.has_parent_path() )
		{
			current_path( config_path.parent_path() );
			log::info( "Path set to ", config_path.parent_path() );
		}

		const PropNode configProp = xo::load_file_with_include( path( config_path.string() ), "INCLUDE" );
		const PropNode& objProp = configProp[ "Optimizer" ][ "Objective" ];
		ObjectiveUP obj = CreateObjective( objProp );
		SimulationObjective& so = dynamic_cast<SimulationObjective&>( *obj );

		// report unused parameters
		LogUntouched( objProp );

		// set data storage
		auto model = so.CreateModelFromParFile( filename );
		model->SetStoreData( true );
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
		obj->WriteResults( path( filename ).replace_extension().str() );

		return statistics;
	}
}
