#include "scone_lua_api.h"
#include "sol/sol.hpp"

namespace scone
{
	void register_lua_wrappers( sol::state& lua )
	{
		lua.new_usertype<lua_vec>( "lua_vec3", sol::constructors<Vec3( double, double, double )>(),
			"x", &lua_vec::x,
			"y", &lua_vec::y,
			"z", &lua_vec::z
			);

		lua.new_usertype<lua_params>( "lua_params", sol::constructors<>(),
			"get", &lua_params::get
			);

		lua.new_usertype<lua_body>( "lua_body", sol::constructors<>(),
			"name", &lua_body::name,
			"com_pos", &lua_body::com_pos,
			"com_vel", &lua_body::com_vel
			);

		lua.new_usertype<lua_actuator>( "lua_actuator", sol::constructors<>(),
			"name", &lua_actuator::name,
			"add_input", &lua_actuator::add_input
			);

		lua.new_usertype<lua_model>( "lua_model", sol::constructors<>(),
			"time", &lua_model::time,
			"actuator", &lua_model::actuator,
			"actuator_count", &lua_model::actuator_count,
			"body", &lua_model::body,
			"body", &lua_model::find_body,
			"body_count", &lua_model::body_count
			);
	}
}
