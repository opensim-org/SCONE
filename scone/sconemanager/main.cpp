#include "SconeManager.h"
#include "scone/core/Log.h"
#include "flut/system_tools.hpp"
#include <boost/filesystem.hpp>

using namespace scone;

int main(int argc, char *argv[])
{
	try
	{
		// error checking
		boost::filesystem::path fileName(argv[1]);
		SCONE_THROW_IF( !boost::filesystem::exists( fileName ), "File does not exist");
		SCONE_THROW_IF( fileName.extension().string() != ".par", "Must give a .par file" );
		
		SconeManager manager;
		manager.SimulateObjective( fileName.string() );
	}
	catch ( std::exception& e )
	{
		log::Critical( e.what() );
		flut::wait_for_key();
	}
}
