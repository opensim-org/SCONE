#include "LuaScript.h"

#include "xo/container/prop_node_tools.h"
#include "scone/core/Log.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/optimization/Params.h"
#include "scone_lua_api.h"

namespace scone
{
	LuaScript::LuaScript( const PropNode& pn, Params& par, Model& model ) :

	INIT_MEMBER_REQUIRED( pn, script_file_ )
	{
		lua_.open_libraries( sol::lib::base, sol::lib::math );
		register_lua_wrappers( lua_ );

		// load script
		auto script = lua_.load_file( FindFile( script_file_ ).str() );
		if ( !script.valid() )
		{
			sol::error err = script;
			SCONE_THROW( "Error in " + script_file_.str() + ":\n:" + err.what() );
		}

		// run once to define functions
		sol::protected_function_result res = script();
		if ( !res.valid() )
		{
			sol::error err = res;
			SCONE_THROW( "Error in " + script_file_.str() + ":\n:" + err.what() );
		}
	}

	LuaScript::~LuaScript()
	{}

	sol::function LuaScript::GetFunction( const String& name )
	{
		sol::function f = lua_[ name ];
		SCONE_THROW_IF( !f.valid(), "Error in " + script_file_.str() + ": Could not find function " + name );
		return f;
	}
}
