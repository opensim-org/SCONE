#include "stdafx.h"
#include "FactoryTest.h"
#include "../sim/simbody/Model_Simbody.h"

namespace scone
{
	namespace cs
	{
		static std::map< std::string, ModelFactory > g_ModelFactories;

		void RegisterFactoryTest()
		{
			// Register derived classes
			g_ModelFactories[ "Simbody" ] = boost::factory< sim::Model_Simbody* >();
		}

		std::map< std::string, ModelFactory >& GetModelFactories()
		{
			return g_ModelFactories;
		}
	}
}
