#pragma once

#include "core.h"
#include "xo/system/settings.h"

namespace scone
{
	class Settings;

	SCONE_API const Settings& GetSconeSettings();
	SCONE_API Settings& UpdateSconeSettings();
	SCONE_API void SaveSconeSettings();

	template< typename T > T GetSconeSetting( const std::string& name ) { return GetSconeSettings().get< T >( name ); }

	class SCONE_API Settings : public xo::settings
	{
	public:
		Settings();
		virtual ~Settings();
	};
}
