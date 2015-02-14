#pragma once

#include "../sim/Model.h"
#include <boost/function.hpp>
#include <boost/functional/factory.hpp>

namespace scone
{
	namespace cs
	{
		// add a model factory
		typedef boost::function< sim::Model*( int ) > ModelFactory;


		std::map< std::string, ModelFactory >& GetModelFactories();

		void RegisterFactoryTest();
	}
}
