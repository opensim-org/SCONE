#pragma once

#include "platform.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"

#include "sol_config.h"

namespace scone
{
	class lua_script
	{
	public:
		lua_script( const path& script_file, const PropNode& props, Params& par, Model& model );
		~lua_script();

		sol::function find_function( const String& name );
		sol::function try_find_function( const String& name );
		xo::path script_file_;

	private:
		sol::state lua_;
	};
}
