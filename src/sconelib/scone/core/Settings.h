#pragma once

#include "platform.h"
#include "xo/system/settings.h"

namespace scone
{
	SCONE_API xo::settings& GetSconeSettings();
	SCONE_API void SaveSconeSettings();
}
