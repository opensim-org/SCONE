#include "stdafx.h"
#include "opt.h"
#include "..\core\Factory.h"
#include "CmaOptimizer.h"

namespace scone
{
	namespace opt
	{
		void RegisterFactoryTypes()
		{
			CmaOptimizer::RegisterFactory();
		}

		OptimizerSP CreateOptimizer( PropNode& prop, const String& key )
		{
			CmaOptimizer::RegisterFactory();
			OptimizerSP opt;
			ProcessProperty( prop, opt, key );
			return opt;
		}
	}
}
