#include "sconeopensim4.h"

#include "scone/core/Factories.h"
#include "ModelOpenSim4.h"

namespace scone
{
	void RegisterSconeOpenSim4()
	{
		GetModelFactory().register_type< ModelOpenSim4 >( "OpenSim4Model" );
		GetModelFactory().register_type< ModelOpenSim4 >( "ModelOpenSim4" );
	}
}
