#include "stdafx.h"
#include "Factories.h"

namespace scone
{
	namespace sim
	{
		static Factory< CreateModelFunc > g_ModelFactory;
		Factory< CreateModelFunc >& GetModelFactory()
		{
			return g_ModelFactory;
		}
	}
}
