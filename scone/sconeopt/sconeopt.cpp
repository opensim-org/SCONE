#include "stdafx.h"

#include <conio.h>

#include "../core/Log.h"
#include "../opt/opt.h"
#include "../cs/cs.h"

using namespace scone;
using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		// register all types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		// get config file
		String config_file( argc > 1 ? String( argv[ 1 ] ) : "config/gait_test.xml" );

		// perform the optimization
		opt::PerformOptimization( config_file );
	}
	catch (std::exception& e)
	{
		SCONE_LOG( "Exception: " << e.what() );		
		_getch();
	}

	return 0;
}
