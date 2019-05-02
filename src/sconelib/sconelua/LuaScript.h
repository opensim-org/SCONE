#pragma once

#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "platform.h"

#include <sol/sol.hpp>

namespace scone
{
	class SCONE_LUA_API LuaScript
	{
	public:
		LuaScript( const PropNode& props, Params& par, Model& model );
		virtual ~LuaScript();

		sol::function GetFunction( const String& name );
		const path& GetFile() const { return script_file_; }

	private:
		sol::state lua_;
		xo::path script_file_;
	};
}
