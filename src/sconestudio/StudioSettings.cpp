#include "StudioSettings.h"

#include "scone/core/system_tools.h"

namespace scone
{
	xo::settings& GetStudioSettings()
	{
		static auto s = xo::settings(
			GetInstallFolder() / "resources/studio-settings-schema.zml",
			GetSettingsFolder() / "studio-settings.zml"
		);
		return s;
	}
}
