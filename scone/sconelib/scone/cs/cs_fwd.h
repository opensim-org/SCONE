#pragma once

#include "scone/core/memory_tools.h"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( Function );

	namespace cs
	{
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
