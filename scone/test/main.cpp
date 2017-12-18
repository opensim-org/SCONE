#include "scone/core/Log.h"
#include "scone/core/system_tools.h"
#include "Test.h"
#include "scone/core/string_tools.h"
#include "xo/system/system_tools.h"
#include <chrono>
#include "xo/time/timer.h"

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
#if defined ( _MSC_VER )
		cout << _MSC_VER << endl;
#endif
	}
	catch ( std::exception& e )
	{
		log::Critical( e.what() );
	}

#ifdef _DEBUG
	xo::wait_for_key();
#endif
	return 0;
}
