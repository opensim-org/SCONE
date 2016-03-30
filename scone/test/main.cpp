// test.cpp : Defines the entry point for the console application.
//



//#include <conio.h>
#include "scone/core/Log.h"
#include "scone/core/system.h"
#include "Test.h"
#include "scone/core/tools.h"
#include "flut/system_tools.hpp"
//#include "scone/core/Angle.h"

using std::cout;
using std::endl;
using namespace scone;

int main( int argc, const char* argv[] )
{
	//Ang< Degrees > a = Ang< Degrees >( 180.0 );
	//Ang< Radians > b = a;

	try
	{
		//DofAxisTest();
		cout << GetDateTimeAsString() << endl;
		cout << GetDateTimeExactAsString() << endl;

		//String fname = GetSconeFolder( "output" ) + "/347.f2354.G_2ML_1MLC_2MLS.S10CW.D5.3DMR11/0000_95.635_94.908.par";
		//PlaybackTest( fname );

		//String fname = GetSconeFolder( "output" ) + "299.f2354.SE2001.FP1.Jump.D1/0997_50.535_50.583.par";
		//SimulationTest();
		//PerformanceTest( fname );
		//SimulationObjectiveTest( argc > 1 ? argv[ 1 ] : "jump_MR.xml" );
	}
	catch ( std::exception& e )
	{
		log::Critical( e.what() );
	}

#ifdef _DEBUG
	flut::wait_for_key();
#endif
	return 0;
}
