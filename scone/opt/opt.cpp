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

		void OPT_API PerformOptimization( const String& config_file )
		{
			// load properties
			PropNode p = ReadPropNode( config_file );

			// set current path to config file path
			path config_path( config_file );
			if ( config_path.has_parent_path() )
				current_path( config_path.parent_path() );

			// create optimizer and report unused parameters
			opt::OptimizerUP o = opt::CreateOptimizer( p.GetChild( "Optimizer" ) );
			p.ToStream( std::cout, "! Unused parameter ", true );

			// copy config and model file
			path outdir( o->GetOutputFolder() );
			copy_file( config_path.filename(), outdir / ( "config" + config_path.extension().string() ), copy_option::overwrite_if_exists );

			o->Run();
		}
	}
}
