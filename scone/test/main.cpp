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
		String fname = GetSconeFolder( "output" ) + "020.f1024.G_3FP0_3RL.S10.D5/2264_39.857_7.188.par";
		PlaybackTest( fname );
		//SimulationTest();

		std::cout << Profiler::GetGlobalInstance().GetReport();
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
