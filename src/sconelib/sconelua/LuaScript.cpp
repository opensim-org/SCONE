#include "LuaScript.h"

#include "xo/container/prop_node_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	
	LuaScript::LuaScript( const PropNode& pn, Params& par, Model& model ) :
	model_( model ),
	par_( par ),
	INIT_MEMBER_REQUIRED( pn, script_file_ )
	{
		log::debug( "Constructing LuaScript ", script_file_ );
		state_.open_libraries( sol::lib::base, sol::lib::math );
		script_ = state_.load_file( script_file_.str() );
	}

	LuaScript::~LuaScript()
	{
		log::debug( "Destructing LuaScript ", script_file_ );
	}

	bool LuaScript::Run()
	{
		sol::protected_function_result res = script_();
		if ( !res.valid() )
		{
			log::error( "Error running ", script_file_ );
			return false;
		}

		return true;
	}
}
