#include "stdafx.h"

#ifdef _MSC_VER

#include <conio.h>

#else

#include <curses.h>
#define _getch() getch()

#endif

#include "../core/Log.h"
#include "../opt/opt.h"
#include "../cs/cs.h"

using namespace scone;
using namespace std;

int main(int argc, char* argv[])
{
	try
	{
		// set log level to trace in debug mode
#ifdef _DEBUG
		log::SetLevel( log::TraceLevel );
#endif

		// register all types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		// perform the optimization
		opt::PerformOptimization( argc, argv );
	}
	catch (std::exception& e)
	{
		log::Critical( e.what() );
		_getch();
	}

	return 0;
}
