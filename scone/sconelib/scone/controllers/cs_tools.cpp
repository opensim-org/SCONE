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
		double result = so->Evaluate();
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

	SimulationObjectiveUP SCONE_API CreateSimulationObjective( const path& filename )
	{
		opt::ParamSet par;
		bfs::path config_path;
		if ( filename.extension() == "par" )
		{
			par.Read( filename );
			config_path = bfs::path( filename.str() ).parent_path() / "config.xml";
		}
		else config_path = filename.str();
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );

		// read properties
		PropNode configProp = flut::load_file_with_include( config_path.string(), "INCLUDE" );
		PropNode objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

		// load init parameters if we just load a config file
		if ( filename.extension() == "xml" || filename.extension() == "scenario" )
		{
			auto& optProp = configProp.get_child( "Optimizer" );
			if ( optProp.get< bool >( "use_init_file" ) )
				par.Read( optProp.get< path >( "init_file" ) );
		}

		// create SimulationObjective object
		SimulationObjectiveUP so = dynamic_unique_cast<SimulationObjective>( CreateObjective( objProp, par ) );

		// report unused parameters
		LogUntouched( objProp );

		return so;
	}
}
