#include "sconelua.h"

#include "scone/core/Factories.h"
#include "lua_script.h"
#include "ScriptController.h"
#include "ScriptMeasure.h"

namespace scone
{
	SCONE_LUA_API void RegisterSconeLua()
	{
		GetControllerFactory().register_type< ScriptController >( "ScriptController" );
		GetMeasureFactory().register_type< ScriptMeasure >( "ScriptMeasure" );
	}
}
