#include "common.h"

#include "scone/optimization/opt_tools.h"
#include "scone/controllers/cs_tools.h"
#include "scone/model/simbody/sim_simbody.h"

struct SconeGlobalFixture
{

    SconeGlobalFixture()
	{
		std::cout << "Initializing SCONE" << std::endl;

		//scone::opt::RegisterFactoryTypes();
		scone::RegisterFactoryTypes();
		scone::sim::RegisterSimbody();
	}

    ~SconeGlobalFixture()
	{
		std::cout << "Cleaning up SCONE" << std::endl;
	}
};

BOOST_GLOBAL_FIXTURE( SconeGlobalFixture );
