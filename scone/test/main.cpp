// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <conio.h>
#include "../core/Log.h"
#include "../core/system.h"
#include "Test.h"
#include "../core/Angle.h"

using std::cout;
using std::endl;
using namespace scone;

int _tmain( int argc, _TCHAR* argv[] )
{
	Ang< Degrees > a = Ang< Degrees >( 180.0 );
	Ang< Radians > b = a;

	try
	{
		//String fname = GetSconeFolder( "output" ) + "299.f2354.SE2001.FP1.Jump.D1/0997_50.535_50.583.par";
		String fname = GetSconeFolder( "output" ) + "297.f1024.G_3FP0_5R.S10CW.D5.GHW14sds2/2922_4.587_0.725.par";

		PlaybackTest( fname );
		//SimulationTest();
		//PerformanceTest( fname );
	}
	catch ( std::exception& e )
	{
		log::Critical( e.what() );
	}

#ifdef _DEBUG
	_getch();
#endif
	return 0;
}
