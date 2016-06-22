#pragma once

#include "scone/core/core.h"
#include "scone/core/PropNode.h"

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

namespace scone
{
	namespace cs
	{
		void SCONE_API RegisterFactoryTypes();
		PropNode SCONE_API RunSimulation( const String& par_file, bool write_results = false );
	}
}
