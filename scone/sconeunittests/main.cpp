#include "optimization_test.h"
#include "propnode_test.h"

int main( int argc, const char* argv[] )
{
	optimization_test();
	propnode_stream_test();

	return XO_TEST_REPORT();
}
