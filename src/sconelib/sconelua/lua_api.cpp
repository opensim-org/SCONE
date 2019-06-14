#include "lua_api.h"
#include "sol/sol.hpp"
#include "xo_lua/lua_register.h"

namespace scone
{
	void register_lua_wrappers( sol::state& lua )
	{
		xo::lua_register_vec3<double>( lua, "vec3" );
		xo::lua_register_quat<double>( lua, "quat" );

		lua.new_usertype<LuaFrame>( "LuaFrame", sol::constructors<>(),
			"set_value", &LuaFrame::set_value,
			"set_bool", &LuaFrame::set_bool,
			"time", &LuaFrame::time
			);

		lua.new_usertype<LuaActuator>( "LuaActuator", sol::constructors<>(),
			"name", &LuaActuator::name,
			"add_input", &LuaActuator::add_input,
			"input", &LuaActuator::input
			);

		lua.new_usertype<LuaDof>( "LuaDof", sol::constructors<>(),
			"name", &LuaDof::name,
			"position", &LuaDof::position,
			"velocity", &LuaDof::velocity
			);

		lua.new_usertype<LuaBody>( "LuaBody", sol::constructors<>(),
			"name", &LuaBody::name,
			"com_pos", &LuaBody::com_pos,
			"com_vel", &LuaBody::com_vel,
			"ang_pos", &LuaBody::ang_pos,
			"ang_vel", &LuaBody::ang_vel,
			"add_external_force", &LuaBody::add_external_force,
			"add_external_moment", &LuaBody::add_external_moment
			);

		lua.new_usertype<LuaMuscle>( "LuaMuscle", sol::constructors<>(),
			"name", &LuaMuscle::name,
			"add_input", &LuaMuscle::add_input,
			"input", &LuaMuscle::input,
			"excitation", &LuaMuscle::excitation,
			"activation", &LuaMuscle::activation,
			"fiber_length", &LuaMuscle::fiber_length,
			"normalized_fiber_length", &LuaMuscle::normalized_fiber_length,
			"optimal_fiber_length", &LuaMuscle::optimal_fiber_length,
			"fiber_force", &LuaMuscle::fiber_force,
			"normalized_fiber_force", &LuaMuscle::normalized_fiber_force,
			"max_isometric_force", &LuaMuscle::max_isometric_force,
			"contraction_velocity", &LuaMuscle::contraction_velocity,
			"normalized_contraction_velocity", &LuaMuscle::normalized_contraction_velocity
			);

		lua.new_usertype<LuaModel>( "LuaModel", sol::constructors<>(),
			"time", &LuaModel::time,
			"delta_time", &LuaModel::delta_time,
			"com_pos", &LuaModel::com_pos,
			"com_vel", &LuaModel::com_vel,
			"actuator", &LuaModel::actuator,
			"find_actuator", &LuaModel::find_actuator,
			"actuator_count", &LuaModel::actuator_count,
			"dof", &LuaModel::dof,
			"find_dof", &LuaModel::find_dof,
			"dof_count", &LuaModel::dof_count,
			"muscle", &LuaModel::muscle,
			"find_muscle", &LuaModel::find_muscle,
			"muscle_count", &LuaModel::muscle_count,
			"body", &LuaModel::body,
			"find_body", &LuaModel::find_body,
			"body_count", &LuaModel::body_count
			);

		lua.new_usertype<LuaParams>( "LuaParams", sol::constructors<>(),
			"create_from_mean_std", &LuaParams::create_from_mean_std,
			"create_from_string", &LuaParams::create_from_string
			);

		lua.new_usertype<LuaScone>( "scone", sol::constructors<>(),
			"trace", &LuaScone::trace,
			"debug", &LuaScone::debug,
			"info", &LuaScone::info,
			"warning", &LuaScone::warning,
			"error", &LuaScone::error
			);
	}
}
