#include "stdafx.h"
#include "Factory.h"

namespace scone
{
	Factory g_Factory;
	Factory& GetFactory() { return g_Factory; }
}
