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
