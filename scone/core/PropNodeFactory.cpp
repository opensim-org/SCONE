#include "stdafx.h"
#include "PropNodeFactory.h"

namespace scone
{
	PropNodeFactory g_Factory;
	PropNodeFactory& GetFactory() { return g_Factory; }
}
