// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <conio.h>
#include "../core/Log.h"
#include "../core/system.h"
#include "Test.h"
#include "../core/Profiler.h"

using std::cout;
using std::endl;
using namespace scone;

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		String fname = GetSconeFolder( "output" ) + "299.f2354.SE2001.FP1.Jump.D1/0997_50.535_50.583.par";
		//PlaybackTest( fname );
		//SimulationTest();
		PerformanceTest( fname );
	}
	catch (std::exception& e)
	{
		log::Critical( e.what() );
	}

#ifdef _DEBUG
	_getch();
#endif
	return 0;
}
