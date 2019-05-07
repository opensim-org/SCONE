#include "ScriptMeasure.h"

#include "lua_script.h"
#include "lua_api.h"
#include "scone/core/Log.h"

namespace scone
{

	ScriptMeasure::ScriptMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	script_( new lua_script( props, par, model ) )
	{
		init_ = script_->find_function( "init" );
		update_ = script_->find_function( "update" );
		result_ = script_->find_function( "result" );

		lua_model lm( model );
		try
		{
			lua_model lm( model );
			init_( &lm );
		}
		catch ( const std::exception& e )
		{
			SCONE_ERROR( "Error in " + script_->script_file_.str() + " while calling init(): " + e.what() );
		}

		model.AddExternalResource( script_->script_file_ );
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
			log::error( "Error in ", script_->script_file_, " while calling result(): ", e.what() );
			return WorstResult();
		}
	}

	bool ScriptMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		try
		{
			lua_model lm( const_cast<Model&>( model ) );
			return update_( &lm );
		}
		catch ( const std::exception& e )
		{
			log::error( "Error in ", script_->script_file_, " while calling update(): ", e.what() );
			return true;
		}
	}

	String ScriptMeasure::GetClassSignature() const
	{
		return "SM";
	}
}
