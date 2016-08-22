#include "scone/core/Log.h"
#include "scone/opt/opt_tools.h"
#include "scone/cs/cs_tools.h"
#include "scone/sim/simbody/sim_simbody.h"
#include "flut/system_tools.hpp"
#include <boost/filesystem.hpp>
#include <tclap/CmdLine.h>

using namespace scone;
using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		TCLAP::CmdLine cmd( "SCONE Command Line Utility", ' ', "0.1", true );
		TCLAP::ValueArg< string > optArg( "o", "optimize", "Scenario to optimize", true, "", "Scenario file" );
		TCLAP::ValueArg< string > resArg( "e", "evaluate", "Evaluate result from an optimization", false, "", "Result file" );
		TCLAP::ValueArg< int > logArg( "l", "log", "Set the log level", false, 1, "1-7", cmd );
		TCLAP::SwitchArg statusOutput( "s", "status", "Output status updates for use in external tools", cmd, false );
		TCLAP::SwitchArg quietOutput( "q", "quiet", "Do not output simulation progress", cmd, false );
		TCLAP::MultiArg< string > propArg( "a", "attribute", "Set specific attribute for scenario", false, "key=value" );
		cmd.xorAdd( optArg, resArg );

		cmd.parse( argc, argv );

		// register all types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();
		sim::RegisterSimbody();

		if ( optArg.isSet() )
		{
			// set log level
			log::SetLevel( logArg.isSet() ? log::Level( logArg.getValue() ) : log::InfoLevel );

			// start optimization
			PropNode pn;
			for ( auto kvstring : propArg )
			{
				auto kvp = flut::to_key_value( kvstring );
				pn.Add( kvp.first, kvp.second );
			}

			opt::OptimizerUP o = opt::PerformOptimization( optArg.getValue(), pn );
			o->SetConsoleOutput( !quietOutput.getValue() );
			o->SetStatusOutput( statusOutput.getValue() );

			if ( o->GetStatusOutput() )
			{
				std::cout << "scenario=" << optArg.getValue() << std::endl;
				std::cout << "folder=" << o->AcquireOutputFolder() << std::endl;
			}

			o->Run();
		}
		else if ( resArg.isSet() )
		{
			// set log level
			log::SetLevel( logArg.isSet() ? log::Level( logArg.getValue() ) : log::TraceLevel );
			opt::SimulateObjective( resArg.getValue() );
		}
		else SCONE_THROW( "Unexpected error parsing program arguments" ); // This should never happen
	}
	catch (std::exception& e)
	{
		log::Critical( e.what() );
		flut::wait_for_key();
	}

	return 0;
}
