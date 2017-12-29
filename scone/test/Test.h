#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"
#include "xo/filesystem/path.h"

namespace scone
{
	void OptimizationTest();
	void DelayTest();
	void ModelTest();
	void PlaybackTest( const xo::path& filename );
	void MuscleLengthTest();
	void DofAxisTest();
}
