/*
** sconecmd.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include <tclap/CmdLine.h>
#include "scone/core/Log.h"
#include "scone/optimization/opt_tools.h"
#include "xo/system/system_tools.h"
#include "xo/system/log_sink.h"
#include "xo/container/prop_node_tools.h"
#include "spot/optimizer_pool.h"
#include "xo/serialization/serialize.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "scone/core/Exception.h"
#include "scone/sconelib_config.h"
#include "scone/core/version.h"

using namespace scone;
using namespace std;

int main(int argc, char* argv[])
{
	xo::log::stream_sink console_sink( xo::log::info_level, std::cout );
	scone::Initialize();

	try
	{
		TCLAP::CmdLine cmd( "SCONE Command Line Utility", ' ', xo::to_str( scone::GetSconeVersion() ), true );
		TCLAP::ValueArg< String > optArg( "o", "optimize", "Optimize a scenario file", true, "", "Scenario file (*.scone)" );
		TCLAP::ValueArg< String > parArg( "e", "evaluate", "Evaluate a result from an optimization", false, "", "Parameter file (*.par)" );
		TCLAP::ValueArg< String > outArg( "r", "result", "Output file for evaluation result", false, "", "Output file (*.sto)", cmd );
		TCLAP::ValueArg< int > logArg( "l", "log", "Set the log level", false, 1, "1-7", cmd );
		TCLAP::SwitchArg statusOutput( "s", "status", "Output full status updates", cmd, false );
		TCLAP::SwitchArg quietOutput( "q", "quiet", "Do not output simulation progress", cmd, false );
		TCLAP::UnlabeledMultiArg< string > propArg( "property", "Override specific scenario property, using <key>=<value>", false, "<key>=<value>", cmd, true );
		cmd.xorAdd( optArg, parArg );
		cmd.parse( argc, argv );
		SCONE_THROW_IF( optArg.isSet() == parArg.isSet(), "Use either -o or -e" );

		try
		{
			// load scenario and add additional properties
			path scenario_file = FindScenario( path( optArg.isSet() ? optArg.getValue() : parArg.getValue() ) );
			PropNode scenario_pn = xo::load_file_with_include( scenario_file, "INCLUDE" );

			// apply command line settings (parameter 2 and further)
			for ( auto kvstring : propArg )
			{
				auto kvp = xo::make_key_value_str( kvstring );
				scenario_pn.set_query( kvp.first, kvp.second, '.' );
			}

			// set log level (optimization defaults to info, evaluation defaults to trace)
			console_sink.set_log_level( xo::log::level( logArg.isSet() ? log::Level( logArg.getValue() ) : optArg.isSet() ? log::InfoLevel : log::TraceLevel ) );

			// do optimization or evaluation
			if ( optArg.isSet() )
			{
				OptimizerUP o = CreateOptimizer( scenario_pn, scenario_file.parent_path() );
				LogUnusedProperties( scenario_pn );
				if ( statusOutput.getValue() )
					o->SetOutputMode( Optimizer::status_output );
				else o->SetOutputMode( quietOutput.getValue() ? Optimizer::no_output : Optimizer::console_output );
				o->Run();
			}
			else
			{
				auto par_path = path( parArg.getValue() );
				auto out_path = path( outArg.isSet() ? outArg.getValue() : parArg.getValue() );
				log::info( "Evaluating ", par_path );
				auto results = EvaluateScenario( scenario_pn, par_path, out_path );
				log::info( results );

				// store config file if arguments have changed
				if ( propArg.isSet() && outArg.isSet() )
					save_file( scenario_pn, out_path.replace_extension( "scone" ) );
			}
		}
		catch ( std::exception& e )
		{
			log::Critical( e.what() );

			if ( statusOutput.isSet() )
			{
				cout << std::endl << "*error=" << xo::try_quoted( e.what() ) << std::endl;
				cout.flush();
				xo::sleep( 5000 );
			}
		}
	}
	catch ( TCLAP::ExitException& e )
	{
		return e.getExitStatus();
	}

	return 0;
}
