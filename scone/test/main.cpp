#include "scone/core/Log.h"
#include "scone/core/system.h"
#include "Test.h"
#include "scone/core/tools.h"
#include "flut/system_tools.hpp"
#include <chrono>
#include "flut/timer.hpp"

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
		cout << _MSC_VER << endl;

		for ( int i = log::TraceLevel; i <= log::CriticalLevel; ++i )
		{
			scone::log::SetLevel( log::Level( i ) );
			log::trace( "trace message ", log::GetLevel() );
			log::info( "info message ", log::GetLevel() );
			log::error( "error message ", log::GetLevel() );
		}

		//flut::timer ft;
		//auto start = std::chrono::high_resolution_clock::now();
		//for ( int i = 0; i < 1000; ++i )
		//{
		//	auto ctime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - start);
		//	auto cticks = (std::chrono::high_resolution_clock::now() - start).count();
		//	auto ftime = ft.seconds();
		//	printf( "%.6f\t%.6f\n", ctime.count(), ftime );
		//}

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
