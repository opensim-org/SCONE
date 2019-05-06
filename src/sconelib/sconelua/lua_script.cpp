#include "lua_script.h"

#include "xo/container/prop_node_tools.h"
#include "scone/core/Log.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/optimization/Params.h"
#include "lua_api.h"

namespace scone
{
	lua_script::lua_script( const PropNode& pn, Params& par, Model& model ) :
	INIT_MEMBER_REQUIRED( pn, script_file_ )
	{
		lua_.open_libraries( sol::lib::base, sol::lib::math );
		register_lua_wrappers( lua_ );

		// propagate all properties to lua script
		for ( auto& prop : pn )
			lua_[ prop.first ] = prop.second.get<string>();

		// load script
		auto script = lua_.load_file( FindFile( script_file_ ).str() );
		if ( !script.valid() )
		{
			sol::error err = script;
			SCONE_ERROR( "Error in " + script_file_.str() + ": " + err.what() );
		}

		// run once to define functions
		sol::protected_function_result res = script();
		if ( !res.valid() )
		{
			sol::error err = res;
			SCONE_ERROR( "Error in " + script_file_.str() + ": " + err.what() );
		}
	}

	lua_script::~lua_script()
	{}

	sol::function lua_script::find_function( const String& name )
	{
		sol::function f = lua_[ name ];
		SCONE_ERROR_IF( !f.valid(), "Error in " + script_file_.str() + ": Could not find function " + xo::quoted( name ) );
		return f;
	}
}
