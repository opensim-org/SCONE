#include "cs_tools.h"

#include "xo/time/timer.h"
#include "xo/stream/prop_node_tools.h"
#include "xo/filesystem/path.h"
#include "PerturbationController.h"
#include "scone/objectives/SimulationObjective.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"

using xo::timer;

namespace scone
{
	PropNode SCONE_API RunSimulation( const path& par_file, bool write_results )
	{
		// create the simulation objective object
		auto mob = CreateModelObjective( par_file );
		auto model = mob->CreateModelFromParFile( par_file );

		// set data storage
		if ( write_results )
			model->SetStoreData( true );

		timer t;
		double result = mob->EvaluateModel( *model );
		auto duration = t.seconds();

		// reset profiler (only if enabled)
		Profiler::GetGlobalInstance().Reset();

		// collect statistics
		PropNode statistics;
		statistics.set( "result", model->GetMeasure()->GetReport() );
		statistics.set( "simulation time", model->GetTime() );
		statistics.set( "performance (x real-time)", model->GetTime() / duration );

		// output profiler results (only if enabled)
		std::cout << Profiler::GetGlobalInstance().GetReport();

		// write results
		if ( write_results )
			mob->WriteResults( xo::path( par_file ).replace_extension().str() );

		return statistics;
	}

	ModelObjectiveUP SCONE_API CreateModelObjective( const path& file )
	{
		bool is_par_file = file.extension() == "par";
		path config_file = is_par_file ? file.parent_path() / "config.xml" : file;

		// read properties
		PropNode configProp = xo::load_file_with_include( config_file, "INCLUDE" );
		PropNode& objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

		// create SimulationObjective object
		auto mob = dynamic_unique_cast< ModelObjective >( CreateObjective( objProp ) );

		if ( !is_par_file )
		{
			// read mean / std from init file
			auto& optProp = configProp.get_child( "Optimizer" );
			if ( optProp.get< bool >( "use_init_file" ) )
			{
				auto init_file = optProp.get< path >( "init_file" );
				auto result = mob->info().import_mean_std( GetFolder( SCONE_SCENARIO_FOLDER ) / init_file, optProp.get< bool >( "use_init_file_std", true ) );
				log::info( "Imported ", result.first, ", skipped ", result.second, " parameters from ", init_file );
			}
		}

		// report unused parameters
		LogUntouched( objProp );

		return mob;
	}
}
