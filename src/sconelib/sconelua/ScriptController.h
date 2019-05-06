#pragma once

#include "platform.h"
#include "scone/controllers/Controller.h"

namespace scone
{
	class SCONE_LUA_API ScriptController : public Controller
	{
	public:
		ScriptController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~ScriptController();

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

		u_ptr< class lua_script > script_;
		std::function<void( struct lua_model*, struct lua_params* )> init_;
		std::function<void( struct lua_model* )> update_;
	};
}
