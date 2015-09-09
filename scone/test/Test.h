#pragma once

#include "../core/core.h"

namespace scone
{
	void OptimizationTest();
	void DelayTest();
	void ModelTest();
	void SimulationObjectiveTest( const String& filename );
	void PlaybackTest( const String& filename );
	void PerformanceTest( const String& filename );
	void XmlParseTest();
	void MuscleLengthTest();
	void DofAxisTest();
}
