#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( Function );

	namespace cs
	{
		void SCONE_API RegisterFactoryTypes();

		SCONE_DECLARE_CLASS_AND_PTR( SimulationObjective );
		SCONE_DECLARE_CLASS_AND_PTR( FeedForwardController );
		SCONE_DECLARE_CLASS_AND_PTR( ReflexController );
		SCONE_DECLARE_CLASS_AND_PTR( MuscleReflex );
		SCONE_DECLARE_CLASS_AND_PTR( Reflex );
		SCONE_DECLARE_CLASS_AND_PTR( Measure );
		SCONE_DECLARE_CLASS_AND_PTR( HeightMeasure );
		SCONE_DECLARE_CLASS_AND_PTR( MetaReflexController );
		SCONE_DECLARE_CLASS_AND_PTR( MetaReflexDof );
		SCONE_DECLARE_CLASS_AND_PTR( MetaReflexMuscle );
		SCONE_DECLARE_CLASS_AND_PTR( MetaReflexVirtualMuscle );
	}
}
