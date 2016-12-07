#include "opt_tools.h"
#include "opt_fwd.h"
#include "CmaOptimizer.h"
#include "Factories.h"

#include "scone/cs/SimulationObjective.h"
#include "scone/core/Profiler.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;

#include "flut/timer.hpp"
#include "flut/prop_node_tools.hpp"

using flut::timer;

namespace scone
{
	namespace opt
	{
		void RegisterFactoryTypes()
		{
			GetOptimizerFactory().Register< CmaOptimizer >();
		}

		SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const String& scenario_file )
		{
			// create optimizer and report unused parameters
			opt::OptimizerUP o = opt::CreateOptimizer( props.get_child( "Optimizer" ) );
			LogUntouched( props );

			// set current path to config file path
			path config_path( scenario_file );
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );

			// copy original and write resolved config files
			path outdir( o->AcquireOutputFolder() );
			copy_file( config_path.filename(), outdir / ( "config_original" + config_path.extension().string() ), copy_option::overwrite_if_exists );
			flut::save_xml( props, ( outdir / "config.xml" ).string() );

			// return created optimizer
			return std::move( o );
		}

		PropNode SCONE_API SimulateObjective( const String& filename )
		{
			cout << "--- Starting evaluation ---" << endl;
	
			opt::ParamSet par( filename );
	
			path config_path = path( filename ).parent_path() / "config.xml";
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );
	
			PropNode configProp = flut::load_file_with_include( config_path.string(), "INCLUDE" ) ;
			const PropNode& objProp = configProp.get_child( "Optimizer.Objective" );
			opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
			cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

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
			obj->WriteResults( path( filename ).replace_extension().string() );

			return statistics;
		}
	}
}
