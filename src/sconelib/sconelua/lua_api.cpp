#include "lua_api.h"
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

		lua.new_usertype<lua_actuator>( "lua_actuator", sol::constructors<>(),
			"name", &lua_actuator::name,
			"add_input", &lua_actuator::add_input,
			"input", &lua_actuator::input
			);

		lua.new_usertype<lua_dof>( "lua_dof", sol::constructors<>(),
			"name", &lua_dof::name,
			"position", &lua_dof::position,
			"velocity", &lua_dof::velocity
			);

		lua.new_usertype<lua_body>( "lua_body", sol::constructors<>(),
			"name", &lua_body::name,
			"com_pos", &lua_body::com_pos,
			"com_vel", &lua_body::com_vel,
			"ang_pos", &lua_body::ang_pos,
			"ang_vel", &lua_body::ang_vel,
			"add_external_force", &lua_body::add_external_force,
			"add_external_moment", &lua_body::add_external_moment
			);

		lua.new_usertype<lua_muscle>( "lua_muscle", sol::constructors<>(),
			"name", &lua_muscle::name,
			"add_input", &lua_muscle::add_input,
			"input", &lua_muscle::input,
			"excitation", &lua_muscle::excitation,
			"activation", &lua_muscle::activation,
			"fiber_length", &lua_muscle::fiber_length,
			"normalized_fiber_length", &lua_muscle::normalized_fiber_length,
			"optimal_fiber_length", &lua_muscle::optimal_fiber_length,
			"fiber_force", &lua_muscle::fiber_force,
			"normalized_fiber_force", &lua_muscle::normalized_fiber_force,
			"max_isometric_force", &lua_muscle::max_isometric_force,
			"contraction_velocity", &lua_muscle::contraction_velocity,
			"normalized_contraction_velocity", &lua_muscle::normalized_contraction_velocity
			);

		lua.new_usertype<lua_model>( "lua_model", sol::constructors<>(),
			"time", &lua_model::time,
			"com_pos", &lua_model::com_pos,
			"com_vel", &lua_model::com_vel,
			"actuator", &lua_model::actuator,
			"find_actuator", &lua_model::find_actuator,
			"actuator_count", &lua_model::actuator_count,
			"dof", &lua_model::dof,
			"find_dof", &lua_model::find_dof,
			"dof_count", &lua_model::dof_count,
			"muscle", &lua_model::muscle,
			"find_muscle", &lua_model::find_muscle,
			"muscle_count", &lua_model::muscle_count,
			"body", &lua_model::body,
			"find_body", &lua_model::find_body,
			"body_count", &lua_model::body_count
			);

		lua.new_usertype<lua_params>( "lua_params", sol::constructors<>(),
			"get", &lua_params::get
			);
	}
}
