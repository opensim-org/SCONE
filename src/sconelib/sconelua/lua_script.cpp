#include "lua_script.h"

#include "xo/container/prop_node_tools.h"
#include "scone/core/Log.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/optimization/Params.h"
#include "lua_api.h"

namespace scone
{
	lua_script::lua_script( const PropNode& pn, Params& par, Model& model )
	{
		lua_.open_libraries( sol::lib::base, sol::lib::math, sol::lib::package );
		register_lua_wrappers( lua_ );

		// find script file (folder can be different if playback)
		script_file_ = FindFile( pn.get<path>( "script_file" ) );
		auto folder = script_file_.has_parent_path() ? script_file_.parent_path() : path( "." );

		// set path for lua modules to current script file folder
		// #todo: add these modules to external resources too!
		lua_[ "package" ][ "path" ] = ( folder / "?.lua" ).c_str();

		// propagate all properties to scone namespace in lua script
		for ( auto& prop : pn )
			lua_[ "scone" ][ prop.first ] = prop.second.get<string>();

		// load script
		auto script = lua_.load_file( script_file_.str() );
		if ( !script.valid() )
		{
			sol::error err = script;
			SCONE_ERROR( "Error in " + script_file_.filename().str() + ": " + err.what() );
		}

		// run once to define functions
		auto res = script();
		if ( !res.valid() )
		{
			sol::error err = res;
			SCONE_ERROR( "Error in " + script_file_.filename().str() + ": " + err.what() );
		}
	}

	lua_script::~lua_script()
	{}

	sol::function lua_script::find_function( const String& name )
	{
		sol::function f = lua_[ name ];
		SCONE_ERROR_IF( !f.valid(), "Error in " + script_file_.filename().str() + ": Could not find function " + xo::quoted( name ) );
		return f;
	}

	sol::function lua_script::try_find_function( const String& name )
	{
		sol::function f = lua_[ name ];
		return f;
	}
}
