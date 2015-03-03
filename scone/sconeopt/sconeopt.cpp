#include "stdafx.h"

#include <conio.h>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "../core/Log.h"
#include "../opt/Optimizer.h"
#include "../cs/SimulationObjective.h"
#include "../opt/Factories.h"
#include "../cs/Polynomial.h"

using namespace boost::filesystem;
using namespace scone;
using namespace std;

PropNode LoadProperties( const path& config_file )
{
	SCONE_LOG( "Reading file " << config_file );

	if ( config_file.extension() == ".xml" )
		return CreatePropNodeFromXmlFile( config_file.string() );
	if ( config_file.extension() == ".info" )
		return CreatePropNodeFromInfoFile( config_file.string() );
	else SCONE_THROW( "Unknown file type: " + config_file.string() );
}

int main(int argc, char* argv[])
{
	try
	{
		// register all types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		// get config file
		path config_file( argc > 1 ? String( argv[ 1 ] ) : "config/jump_test.xml" );

		// load properties
		PropNode p = LoadProperties( config_file );

		// create optimizer and report unused parameters
		opt::OptimizerUP o = opt::CreateOptimizer( p.GetChild( "Optimizer" ) );
		p.ToStream( std::cout, "Unused parameter ", true );

		// copy config and model file
		path outdir( o->GetOutputFolder() );
		copy_file( config_file, outdir / ( "config" + config_file.extension().string() ), copy_option::overwrite_if_exists );

		o->Run();
	}
	catch (std::exception& e)
	{
		SCONE_LOG( "Exception: " << e.what() );		
		_getch();
	}

	return 0;
}
