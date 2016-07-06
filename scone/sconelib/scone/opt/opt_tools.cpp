#include "opt_tools.h"
#include "opt_fwd.h"
#include "CmaOptimizer.h"
#include "Factories.h"

#include "scone/cs/SimulationObjective.h"
#include "scone/core/Profiler.h"

#include <boost/filesystem.hpp>

using namespace boost::filesystem;
using namespace std;

#include <flut/timer.hpp>
using flut::timer;

namespace scone
{
	namespace opt
	{
		void RegisterFactoryTypes()
		{
			GetOptimizerFactory().Register< CmaOptimizer >();
		}

		void SCONE_API PerformOptimization( int argc, char* argv[] )
		{
			SCONE_THROW_IF( argc < 2, "No config file argument provided" );

			// get config file
			String config_file( argv[ 1 ] );

			// load properties
			PropNode props = ReadPropNode( config_file );

			// get command line settings (parameter 2 and further)
			PropNode cmd_props = GetPropNodeFromArgs( 2, argc, argv );
			if ( !cmd_props.IsEmpty() )
			{
				//log::Info( "Command line properties:" );
				//std::cout << cmd_props;
				props.Merge( cmd_props, true );
			}

			// set current path to config file path
			path config_path( config_file );
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );

			// create optimizer and report unused parameters
			opt::OptimizerUP o = opt::CreateOptimizer( props.GetChild( "Optimizer" ) );
			LogUntouched( props );

			// copy original and write resolved config files
			path outdir( o->AcquireOutputFolder() );
			copy_file( config_path.filename(), outdir / ( "config_original" + config_path.extension().string() ), copy_option::overwrite_if_exists );
			props.ToXmlFile( ( outdir / "config.xml" ).string() );

			o->Run();
		}

		PropNode SCONE_API SimulateObjective( const String& filename )
		{
			cout << "--- Starting evaluation ---" << endl;
	
			opt::ParamSet par( filename );
	
			path config_path = path( filename ).parent_path() / "config.xml";
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );
	
			PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
			const PropNode& objProp = configProp.GetChild( "Optimizer.Objective" );
			opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
			cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

			// report unused parameters
			LogUntouched( objProp );

			// set data storage
			so.GetModel().SetStoreData( true );
		
			Profiler::GetGlobalInstance().Reset();

			PropNode statistics;
			statistics.Clear();
			timer tmr;
			double result = obj->Evaluate();
			auto duration = tmr.seconds();
	
			// collect statistics
			statistics.Clear();
			statistics.Set( "result", result );
			statistics.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
			statistics.Set( "simulation time", so.GetModel().GetTime() );
			statistics.Set( "performance (x real-time)", so.GetModel().GetTime() / duration );
	
			cout << "--- Evaluation report ---" << endl;
			cout << statistics << endl;

			cout << Profiler::GetGlobalInstance().GetReport();

			// write results
			obj->WriteResults( path( filename ).replace_extension().string() );

			return statistics;
		}
	}
}
