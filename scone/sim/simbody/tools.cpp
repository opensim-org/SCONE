#include "stdafx.h"

#include "tools.h"

namespace scone
{
	template <> OpenSim::Model* ResourceCache< OpenSim::Model >::CreateFirst( const String& name )
	{
		return new OpenSim::Model( name );
	}

	template <> OpenSim::Storage* ResourceCache< OpenSim::Storage >::CreateFirst( const String& name )
	{
		return new OpenSim::Storage( name );
	}

	namespace sim
	{
		//ResourceCache< OpenSim::Model > g_ModelCache;
	}
}
