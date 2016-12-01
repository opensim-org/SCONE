#pragma once

#include "scone/core/core.h"
#include "scone/core/String.h"

namespace scone
{
	void OptimizationTest();
	void DelayTest();
	void ModelTest();
	void SimulationObjectiveTest( const String& filename );
	void PlaybackTest( const String& filename );
	void PerformanceTest( const String& filename );
	void MuscleLengthTest();
	void DofAxisTest();
}
