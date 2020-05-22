/*
** sconecmd.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include <tclap/CmdLine.h>
#include "scone/core/Exception.h"
#include "scone/core/Factories.h"
#include "scone/core/Log.h"
#include "scone/core/version.h"
#include "scone/optimization/opt_tools.h"
#include "scone/sconelib_config.h"
#include "spot/optimizer_pool.h"
#include "xo/container/prop_node_tools.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "xo/serialization/serialize.h"
#include "xo/system/log_sink.h"
#include "xo/system/system_tools.h"

using namespace scone;

// load scenario and handle custom arguments
PropNode load_scenario( const path& scenario_file, const TCLAP::UnlabeledMultiArg< string >& propArg )
{
	PropNode scenario_pn = xo::load_file_with_include( scenario_file, "INCLUDE" );
	for ( auto kvstring : propArg ) {
		auto kvp = xo::make_key_value_str( kvstring );
		scenario_pn.set_query( kvp.first, kvp.second, '.' );
	}
	return scenario_pn;
}

// main
int main(int argc, char* argv[])
{
	xo::log::console_sink console_sink( xo::log::level::info );
	scone::Initialize();

	try
	{
		TCLAP::CmdLine cmd( "SCONE Command Line Utility", ' ', xo::to_str( scone::GetSconeVersion() ), true );
		TCLAP::ValueArg< String > optArg( "o", "optimize", "Optimize a scenario file", true, "", "*.scone" );
		TCLAP::ValueArg< String > parArg( "e", "evaluate", "Evaluate a result from an optimization", false, "", "*.par" );
		TCLAP::ValueArg< String > benchArg( "b", "benchmark", "Benchmark a scenario or parameter file", false, "", "*.scone" );
		TCLAP::ValueArg< String > outArg( "r", "result", "Output file for evaluation result", false, "", "Output file (*.sto)", cmd );
		TCLAP::ValueArg< int > logArg( "l", "log", "Set the log level", false, 1, "1-7", cmd );
		TCLAP::SwitchArg statusOutput( "s", "status", "Output full status updates", cmd, false );
		TCLAP::SwitchArg quietOutput( "q", "quiet", "Do not output simulation progress", cmd, false );
		TCLAP::UnlabeledMultiArg< string > propArg( "property", "Override specific scenario property, using <key>=<value>", false, "<key>=<value>", cmd, true );

		auto xor_args = std::vector<TCLAP::Arg*>{ &optArg, &parArg , &benchArg };
		cmd.xorAdd( xor_args );
		cmd.parse( argc, argv );

		try
		{
			// set log level (optimization defaults to info, evaluation defaults to trace)
			if ( logArg.isSet() )
				console_sink.set_log_level( xo::log::level( logArg.getValue() ) );

			// do optimization or evaluation
			if ( optArg.isSet() )
			{
				path scenario_file = FindScenario( optArg.getValue() );
				auto scenario_pn = load_scenario( scenario_file, propArg );
				OptimizerUP o = CreateOptimizer( scenario_pn, scenario_file.parent_path() );
				LogUnusedProperties( scenario_pn );
				if ( statusOutput.getValue() )
					o->SetOutputMode( Optimizer::status_console_output );
				else o->SetOutputMode( quietOutput.getValue() ? Optimizer::no_output : Optimizer::console_output );
				o->Run();
			}
			else if ( parArg.isSet() )
			{
				path scenario_file = FindScenario( parArg.getValue() );
				auto scenario_pn = load_scenario( scenario_file, propArg );
				auto out_path = path( outArg.isSet() ? outArg.getValue() : parArg.getValue() );
				log::info( "Evaluating ", parArg.getValue() );
				auto results = EvaluateScenario( scenario_pn, parArg.getValue(), out_path );
				log::info( results );

				// store config file if arguments have changed
				if ( propArg.isSet() && outArg.isSet() )
					save_file( scenario_pn, out_path.replace_extension( "scone" ) );
			}
			else if ( benchArg.isSet() )
			{
				path scenario_file = FindScenario( benchArg.getValue() );
				auto scenario_pn = load_scenario( scenario_file, propArg );
				log::info( "Benchmarking ", benchArg.getValue() );
				BenchmarkScenario( scenario_pn, path( benchArg.getValue() ), 12 );
			}
		}
		catch ( std::exception& e )
		{
			log::Critical( e.what() );

			if ( statusOutput.isSet() )
			{
				std::cout << std::endl << "*error=" << xo::try_quoted( e.what() ) << std::endl;
				std::cout.flush();
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
