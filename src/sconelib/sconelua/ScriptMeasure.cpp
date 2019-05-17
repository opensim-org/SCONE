#include "ScriptMeasure.h"

#include "lua_script.h"
#include "lua_api.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"

namespace scone
{
	ScriptMeasure::ScriptMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	script_( new lua_script( props, par, model ) )
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
			lua_model lm( model );
			init_( &lm );
		}

		model.AddExternalResource( script_->script_file_ );
	}

	ScriptMeasure::~ScriptMeasure()
	{}

	double ScriptMeasure::ComputeResult( Model& model )
	{
		lua_model lm( const_cast<Model&>( model ) );
		return result_( &lm );
	}

	bool ScriptMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		if ( update_ )
		{
			lua_model lm( const_cast<Model&>( model ) );
			return update_( &lm );
		}
		else return false;
	}

	void ScriptMeasure::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		SCONE_PROFILE_FUNCTION;

		if ( store_ )
		{
			lua_frame lf( frame );
			store_( &lf );
		}
	}

	String ScriptMeasure::GetClassSignature() const
	{
		return "SM";
	}
}
