#include "sconeopensim4.h"

#include "scone/core/Factories.h"
#include "ModelOpenSim4.h"

namespace scone
{
	SCONE_OPENSIM_4_API void RegisterSconeOpenSim4()
	{
		GetModelFactory().register_type< ModelOpenSim4 >( "ModelOpenSim4" );
	}
}
