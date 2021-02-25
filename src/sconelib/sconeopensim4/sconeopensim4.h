#pragma once

#include "platform.h"
#include "scone/core/types.h"

namespace scone
{
	SCONE_OPENSIM_4_API void RegisterSconeOpenSim4();
	SCONE_OPENSIM_4_API void ConvertModelOpenSim4( const xo::path& inputFile, const xo::path& outputFile );
}
