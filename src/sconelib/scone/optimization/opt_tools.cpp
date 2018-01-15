#include "opt_tools.h"
#include "scone/core/types.h"

#include "scone/core/Factories.h"

#include "scone/objectives/SimulationObjective.h"
#include "scone/core/Profiler.h"

using namespace std;

#include "xo/time/timer.h"
#include "xo/stream/prop_node_tools.h"
#include "xo/filesystem/filesystem.h"

using xo::timer;

namespace scone
{
	SCONE_API OptimizerUP PrepareOptimization( const PropNode& props, const path& scenario_file )
	{
		// create optimizer and report unused parameters
		OptimizerUP o = CreateOptimizer( props.get_child( "Optimizer" ) );
		LogUntouched( props );

		// set current path to config file path
		xo::path config_path( scenario_file.str() );
		if ( config_path.has_parent_path() )
			xo::current_path( config_path.parent_path() );

		// copy original and write resolved config files
		xo::path outdir( o->AcquireOutputFolder().str() );
		xo::copy_file( config_path.filename(), outdir / path( "config_original" ).replace_extension( config_path.extension() ), true );
		xo::save_xml( props, path( ( outdir / "config.xml" ).string() ) );

		// copy model to output folder
		xo::path modelfile = props.get_delimited< path >( "Optimizer.Objective.Model.model_file" );
		xo::copy_file( GetFolder( SCONE_MODEL_FOLDER ) / modelfile, outdir / modelfile.filename(), true );

		// return created optimizer
		return std::move( o );
	}

	PropNode SCONE_API SimulateObjective( const path& filename )
	{
		cout << "--- Starting evaluation ---" << endl;

		xo::path config_path = xo::path( filename.str() ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			current_path( config_path.parent_path() );

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
		statistics.set( "result", model->GetMeasure()->GetReport() );
		statistics.set( "simulation time", model->GetTime() );
		statistics.set( "performance (x real-time)", model->GetTime() / duration );

		cout << "--- Evaluation report ---" << endl;
		cout << statistics << endl;

		cout << Profiler::GetGlobalInstance().GetReport();

		// write results
		obj->WriteResults( path( filename ).replace_extension().str() );

		return statistics;
	}
}
