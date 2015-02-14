#pragma once

#include "Model.h"
#include <boost/function.hpp>
#include "../core/Factory.h"

namespace scone
{
	namespace sim
	{
		typedef boost::function< Model*( const PropNode& ) > CreateModelFunc;
		SCONE_SIM_API Factory< CreateModelFunc >& GetModelFactory();
	}
}
