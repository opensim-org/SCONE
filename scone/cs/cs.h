#pragma once

#include "../core/core.h"

#ifdef CS_EXPORTS
#define CS_API __declspec(dllexport)
#else
#define CS_API __declspec(dllimport)
#endif

// TODO: do this in a nicer way (i.e. push/pop warnings)
#pragma warning( disable: 4251 )

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( Function );

	namespace cs
	{
		void CS_API RegisterFactoryTypes();

		SCONE_DECLARE_CLASS_AND_PTR( SimulationObjective );
		SCONE_DECLARE_CLASS_AND_PTR( FeedForwardController );
		SCONE_DECLARE_CLASS_AND_PTR( ReflexController );
		SCONE_DECLARE_CLASS_AND_PTR( Reflex );
		SCONE_DECLARE_CLASS_AND_PTR( Measure );
		SCONE_DECLARE_CLASS_AND_PTR( HeightMeasure );
	}
}
