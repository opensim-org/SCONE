#pragma once

#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "platform.h"

#include <sol/sol.hpp>
#include "model_wrapper.h"

namespace scone
{
	class SCONE_LUA_API LuaScript
	{
	public:
		LuaScript( const PropNode& props, Params& par, Model& model );
		virtual ~LuaScript();

		/// add a value to actuator index [1..actuator_count]
		void add_actuator_input( int index, double value );

		bool Run();

	private:
		string GetErrorMsg( const sol::protected_function_result& res );
		Params& par_;
		Model& model_;

		sol::state lua_;
		sol::load_result script_;
		xo::path script_file_;
		model_wrapper lua_model_;
	};	
}
