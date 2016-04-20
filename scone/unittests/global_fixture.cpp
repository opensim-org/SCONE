#include "common.h"

#include "scone/opt/opt.h"
#include "scone/cs/cs.h"
#include "scone/sim/simbody/sim_simbody.h"

struct SconeGlobalFixture
{

    SconeGlobalFixture()
	{
		std::cout << "Initializing SCONE" << std::endl;

		scone::opt::RegisterFactoryTypes();
		scone::cs::RegisterFactoryTypes();
		scone::sim::RegisterSimbody();

		scone::log::SetLevel( scone::log::WarningLevel );
	}

    ~SconeGlobalFixture()
	{
		std::cout << "Cleaning up SCONE" << std::endl;
	}
};

BOOST_GLOBAL_FIXTURE( SconeGlobalFixture );
