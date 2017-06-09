#include "cs_tools.h"

#include <boost/filesystem.hpp>
#include <flut/timer.hpp>
#include "flut/prop_node_tools.hpp"
#include "PerturbationController.h"
#include "scone/objectives/SimulationObjective.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"

using flut::timer;
namespace bfs = boost::filesystem;

namespace scone
{
	PropNode SCONE_API RunSimulation( const path& par_file, bool write_results )
	{
		// create the simulation objective object
		SimulationObjectiveUP so = CreateSimulationObjective( par_file );

		timer t;
		//double result = so->Evaluate();
		auto duration = t.seconds();

		// set data storage
		if ( write_results )
			so->GetModel().SetStoreData( true );

		// reset profiler (only if enabled)
		Profiler::GetGlobalInstance().Reset();

		// collect statistics
		PropNode statistics;
		statistics.set( "result", so->GetMeasure().GetReport() );
		statistics.set( "simulation time", so->GetModel().GetTime() );
		statistics.set( "performance (x real-time)", so->GetModel().GetTime() / duration );

		// output profiler results (only if enabled)
		std::cout << Profiler::GetGlobalInstance().GetReport();

		// write results
		if ( write_results )
			so->WriteResults( bfs::path( par_file.str() ).replace_extension().string() );

		return statistics;
	}

	SimulationObjectiveUP SCONE_API CreateSimulationObjective( const path& file )
	{
		bool is_par_file = file.extension() == "par";
		path config_file = is_par_file ? file.parent_path() / "config.xml" : file;

		// read properties
		PropNode configProp = flut::load_file_with_include( config_file.str(), "INCLUDE" );
		PropNode& objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

		// create SimulationObjective object
		SimulationObjectiveUP so = dynamic_unique_cast<SimulationObjective>( CreateObjective( objProp ) );

		if ( !is_par_file )
		{
			// read mean / std from init file
			auto& optProp = configProp.get_child( "Optimizer" );
			if ( optProp.get< bool >( "use_init_file" ) )
				so->info().import_mean_std( optProp.get< path >( "init_file" ), optProp.get< bool >( "use_init_file_std", true ) );
		}

		if ( is_par_file )
			so->CreateModelFromParameters( ParamInstance( so->info(), file ) );

		// report unused parameters
		LogUntouched( objProp );

		return so;
	}
}
