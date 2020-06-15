#include "sconeopensim3.h"

#include "scone/core/Factories.h"
#include "ModelOpenSim3.h"

namespace scone
{
	SCONE_OPENSIM_3_API void RegisterSconeOpenSim3()
	{
		GetModelFactory().register_type< ModelOpenSim3 >( "Simbody" ); // backwards compatibility
		GetModelFactory().register_type< ModelOpenSim3 >( "OpenSim3Model" );
		GetModelFactory().register_type< ModelOpenSim3 >( "OpenSimModel" );
		GetModelFactory().register_type< ModelOpenSim3 >( "ModelOpenSim3" );
	}
}
