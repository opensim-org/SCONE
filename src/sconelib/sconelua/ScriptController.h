#pragma once

#include "platform.h"
#include "scone/controllers/Controller.h"

#include <sol/sol.hpp>

namespace scone
{
	class SCONE_LUA_API ScriptController : public Controller
	{
	public:
		ScriptController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~ScriptController() {}

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

		sol::state lua_;
	};
}
