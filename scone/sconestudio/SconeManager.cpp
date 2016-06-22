#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "SconeManager.h"
#include "scone/opt/opt_tools.h"
#include "scone/cs/cs_tools.h"
#include "scone/core/system_tools.h"
#include "scone/opt/ParamSet.h"
#include "scone/sim/Factories.h"
#include "scone/opt/Factories.h"
#include "scone/core/string_tools.h"

#include "scone/sim/simbody/sim_simbody.h"

using namespace boost::filesystem;
using boost::format;
using namespace std;

namespace scone
{
	SconeManager::SconeManager()
	{
		// set log level
		log::SetLevel( log::TraceLevel );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();
		sim::RegisterSimbody();
	}

	SconeManager::~SconeManager()
	{
	}

	void SconeManager::SimulateObjective( const String& filename )
	{
		m_Statistics = opt::SimulateObjective( filename );
	}
}
