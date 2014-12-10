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

	}
}
