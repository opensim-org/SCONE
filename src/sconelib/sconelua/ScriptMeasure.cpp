#include "ScriptMeasure.h"

#include "LuaScript.h"
#include "scone_lua_api.h"
#include "scone/core/Log.h"

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
		try
		{
			lua_model lm( model );
			init_( &lm );
		}
		catch ( const std::exception& e )
		{
			SCONE_ERROR( "Error in " + script_->GetFile().str() + " while calling init(): " + e.what() );
		}

		model.AddExternalResource( script_->GetFile() );
	}

	ScriptMeasure::~ScriptMeasure()
	{}

	double ScriptMeasure::ComputeResult( Model& model )
	{
		try
		{
			lua_model lm( const_cast<Model&>( model ) );
			return result_( &lm );
		}
		catch ( const std::exception& e )
		{
			log::error( "Error in ", script_->GetFile().str(), " while calling result(): ", e.what() );
			return WorstResult();
		}
	}

	bool ScriptMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		try
		{
			lua_model lm( const_cast<Model&>( model ) );
			update_( &lm );
		}
		catch ( const std::exception& e )
		{
			log::error( "Error in ", script_->GetFile().str(), " while calling update(): ", e.what() );
			return true;
		}

		return false;
	}

	String ScriptMeasure::GetClassSignature() const
	{
		return "SM";
	}
}
