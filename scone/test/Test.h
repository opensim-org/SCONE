#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"
#include "flut/system/path.hpp"

namespace scone
{
	void OptimizationTest();
	void DelayTest();
	void ModelTest();
	void PlaybackTest( const flut::path& filename );
	void MuscleLengthTest();
	void DofAxisTest();
}
