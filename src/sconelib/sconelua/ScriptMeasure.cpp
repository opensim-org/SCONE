#include "ScriptMeasure.h"

#include "LuaScript.h"
#include "scone_lua_api.h"

namespace scone
{

	ScriptMeasure::ScriptMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	script_( new LuaScript( props, par, model ) )
	{
		init_ = script_->GetFunction( "init" );
		update_ = script_->GetFunction( "update" );
		result_ = script_->GetFunction( "result" );

		lua_model lm( model );
		init_( &lm );

		model.AddExternalResource( script_->GetFile() );
	}

	ScriptMeasure::~ScriptMeasure()
	{}

	double ScriptMeasure::ComputeResult( Model& model )
	{
		lua_model lm( const_cast<Model&>( model ) );
		double res = result_( &lm );
		return res;
	}

	bool ScriptMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		lua_model lm( const_cast<Model&>( model ) );
		double res = update_( &lm );
		return res;
	}

	String ScriptMeasure::GetClassSignature() const
	{
		return "SM";
	}
}
