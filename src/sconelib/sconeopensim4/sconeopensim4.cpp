#include "sconeopensim3.h"

#include "scone/core/Factories.h"
#include "ModelOpenSim4.h"

namespace scone
{
	SCONE_OPENSIM_4_API void RegisterSconeOpenSim4()
	{
		GetModelFactory().register_type< ModelOpenSim4 >( "Simbody" ); // backwards compatibility
		GetModelFactory().register_type< ModelOpenSim4 >( "OpenSim4Model" );
		GetModelFactory().register_type< ModelOpenSim4 >( "OpenSimModel" );
		GetModelFactory().register_type< ModelOpenSim4 >( "ModelOpenSim4" );
	}
}
