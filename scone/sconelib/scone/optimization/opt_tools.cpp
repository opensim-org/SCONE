#include "opt_tools.h"
#include "opt_fwd.h"
#include "CmaOptimizer.h"
#include "Factories.h"

#include "scone/objectives/SimulationObjective.h"
#include "scone/core/Profiler.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;
using namespace std;

#include "flut/timer.hpp"
#include "flut/prop_node_tools.hpp"

#include "CmaOptimizerShark3.h"
#include "CmaOptimizerCCMAES.h"

using flut::timer;

namespace scone
{
	namespace opt
	{
		SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const path& scenario_file )
		{
			// create optimizer and report unused parameters
			opt::OptimizerUP o = opt::CreateOptimizer( props.get_child( "Optimizer" ) );
			LogUntouched( props );

			// set current path to config file path
			bfs::path config_path( scenario_file.str() );
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );

			// copy original and write resolved config files
			bfs::path outdir( o->AcquireOutputFolder() );
			bfs::copy_file( config_path.filename(), outdir / ( "config_original" + config_path.extension().string() ), bfs::copy_option::overwrite_if_exists );
			flut::save_xml( props, ( outdir / "config.xml" ).string() );

			// return created optimizer
			return std::move( o );
		}

		PropNode SCONE_API SimulateObjective( const path& filename )
		{
			cout << "--- Starting evaluation ---" << endl;
	
			opt::ParamSet par( filename );
	
			bfs::path config_path = bfs::path( filename.str() ).parent_path() / "config.xml";
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );
	
			const PropNode configProp = flut::load_file_with_include( config_path.string(), "INCLUDE" ) ;
			const PropNode& objProp = configProp[ "Optimizer" ][ "Objective" ];
			opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
			SimulationObjective& so = dynamic_cast< SimulationObjective& >( *obj );

			// report unused parameters
			LogUntouched( objProp );

			// set data storage
			so.GetModel().SetStoreData( true );
		
			Profiler::GetGlobalInstance().Reset();

			timer tmr;
			double result = obj->Evaluate();
			auto duration = tmr.seconds();
	
			// collect statistics
			PropNode statistics;
			statistics.set( "result", so.GetMeasure().GetReport() );
			statistics.set( "simulation time", so.GetModel().GetTime() );
			statistics.set( "performance (x real-time)", so.GetModel().GetTime() / duration );
	
			cout << "--- Evaluation report ---" << endl;
			cout << statistics << endl;

			cout << Profiler::GetGlobalInstance().GetReport();

			// write results
			obj->WriteResults( path( filename ).replace_extension().str() );

			return statistics;
		}
	}
}
