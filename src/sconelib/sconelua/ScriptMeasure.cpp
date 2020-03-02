#include "ScriptMeasure.h"

#include "lua_script.h"
#include "lua_api.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"

namespace scone
{
	ScriptMeasure::ScriptMeasure( const PropNode& pn, Params& par, const Model& model, const Location& loc ) :
		Measure( pn, par, model, loc ),
		script_file( FindFile( pn.get<path>( "script_file" ) ) ),
		INIT_MEMBER( pn, external_files, std::vector<path>() ),
		script_( new lua_script( script_file, pn, par, const_cast<Model&>( model ) ) ) // const_cast is needed because Lua doesn't care about const
	{
		// optional functions
		if ( auto f = script_->try_find_function( "init" ) )
			init_ = f;
		if ( auto f = script_->try_find_function( "update" ) )
			update_ = f;
		if ( auto f = script_->try_find_function( "store_data" ) )
			store_ = f;

		// result is required
		result_ = script_->find_function( "result" );

		if ( init_ )
		{
			LuaModel lm( const_cast<Model&>( model ) );
			init_( &lm );
		}

		// add lua files as external resources
		model.AddExternalResource( script_->script_file_ );
		for ( auto& f : external_files )
			model.AddExternalResource( FindFile( f ) );
	}

	double ScriptMeasure::ComputeResult( const Model& model )
	{
		LuaModel lm( const_cast<Model&>( model ) );
		return result_( &lm );
	}

	bool ScriptMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		if ( update_ )
		{
			LuaModel lm( const_cast<Model&>( model ) );
			return update_( &lm );
		}
		else return false;
	}

	void ScriptMeasure::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		SCONE_PROFILE_FUNCTION;

		if ( store_ )
		{
			LuaFrame lf( frame );
			store_( &lf );
		}
	}

	String ScriptMeasure::GetClassSignature() const
	{
		return "SM";
	}
}
