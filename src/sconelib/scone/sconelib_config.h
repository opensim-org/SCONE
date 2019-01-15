#pragma once

#include "scone/core/Factories.h"

#ifdef SCONE_OPENSIM_3
#include "sconeopensim3/ModelOpenSim3.h"
#endif

#ifdef SCONE_OPENSIM_4
#include "sconeopensim4/ModelOpenSim3.h"
#endif

namespace scone
{
	void RegisterModels()
	{
#ifdef SCONE_OPENSIM_3
	GetModelFactory().register_class< scone::ModelOpenSim3 >( "OpenSim3" );
	GetModelFactory().register_class< scone::ModelOpenSim3 >( "OpenSim3" );
#endif
#ifdef SCONE_OPENSIM_4
	GetModelFactory().register_class< scone::ModelOpenSim34 >( "OpenSim4" );
#endif
	}
}
