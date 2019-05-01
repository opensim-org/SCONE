#include "LuaScript.h"

#include "xo/container/prop_node_tools.h"
#include "scone/core/Log.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/optimization/Params.h"
#include <functional>

namespace scone
{
	using namespace std::placeholders;

	LuaScript::LuaScript( const PropNode& pn, Params& par, Model& model ) :
	model_( model ),
	lua_model_( model ),
	par_( par ),
	INIT_MEMBER_REQUIRED( pn, script_file_ )
	{
		log::debug( "Constructing LuaScript ", script_file_ );
		lua_.open_libraries( sol::lib::base, sol::lib::math );

		lua_.new_usertype<Vec3>( "Vec3", sol::constructors<Vec3( double, double, double )>(),
			"x", &Vec3::x,
			"y", &Vec3::y,
			"z", &Vec3::z
			);

		lua_.new_usertype<Body>( "Body", sol::constructors<>(),
			"GetComPos", &Body::GetComPos );

		lua_.new_usertype<Actuator>( "lua_actuator2", sol::constructors<>(),
			"add_input", &Actuator::AddInput );

		lua_.new_usertype<model_wrapper>( "lua_model", sol::constructors<>(),
			"get_time", &model_wrapper::get_time,
			"get_actuator", &model_wrapper::get_actuator,
			"get_body", &model_wrapper::get_body
			);

		lua_[ "model" ] = &lua_model_;

		// load script
		script_ = lua_.load_file( script_file_.str() );
		if ( !script_.valid() )
		{
			sol::error err = script_;
			SCONE_THROW( err.what() );
		}

		// run once
		sol::protected_function_result res = script_();
		if ( !res.valid() )
			SCONE_THROW( GetErrorMsg( res ) );
	}

	LuaScript::~LuaScript()
	{
		log::debug( "Destructing LuaScript ", script_file_ );
	}

	void LuaScript::add_actuator_input( int index, double value )
	{
		model_.GetActuators()[ index - 1 ]->AddInput( value );
	}

	bool LuaScript::Run()
	{
		sol::protected_function_result res = script_();
		if ( !res.valid() )
		{
			log::warning( GetErrorMsg( res ) );
			return false;
		}

		return true;
	}

	string LuaScript::GetErrorMsg( const sol::protected_function_result& res )
	{
		if ( res.valid() )
			return "No Error";

		sol::error err = res;
		return "Error in " + script_file_.str() + ":\n:" + err.what();
	}
}
