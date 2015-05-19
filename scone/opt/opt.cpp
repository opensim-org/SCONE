#include "stdafx.h"
#include "opt.h"
#include "CmaOptimizer.h"
#include "Factories.h"

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

namespace scone
{
	namespace opt
	{
		void RegisterFactoryTypes()
		{
			GetOptimizerFactory().Register< CmaOptimizer >();
		}

		void OPT_API PerformOptimization( int argc, char* argv[] )
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
	}
}
