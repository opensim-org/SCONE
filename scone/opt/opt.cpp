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
			PropNode p = ReadProperties( config_file );

			// get config file
			path config_path( config_file );

			// create optimizer and report unused parameters
			opt::OptimizerUP o = opt::CreateOptimizer( p.GetChild( "Optimizer" ) );
			p.ToStream( std::cout, "! Unused parameter ", true );

			// copy config and model file
			path outdir( o->GetOutputFolder() );
			copy_file( config_file, outdir / ( "config" + config_path.extension().string() ), copy_option::overwrite_if_exists );

			o->Run();
		}
	}
}
