#pragma once

#include "core.h"
#include "xo/system/settings.h"

namespace scone
{
	class Settings;

	SCONE_API const Settings& GetSconeSettings();
	SCONE_API Settings& UpdateSconeSettings();
	SCONE_API void SaveSconeSettings();

	class SCONE_API Settings : public xo::settings
	{
	public:
		Settings();
		virtual ~Settings();
	};
}
