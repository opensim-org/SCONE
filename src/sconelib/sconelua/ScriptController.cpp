/*
** NoiseController.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ScriptController.h"
#include "scone/model/Model.h"
#include "scone/model/Actuator.h"
#include "scone/core/string_tools.h"
#include "lua_script.h"
#include "lua_api.h"
#include "scone/core/Log.h"
#include "scone/core/profiler_config.h"

namespace scone
{
	ScriptController::ScriptController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc ),
	script_( new lua_script( props, par, model ) )
	{
		// optional functions
		if ( auto f = script_->try_find_function( "init" ) )
			init_ = f;
		if ( auto f = script_->try_find_function( "store_data" ) )
			store_ = f;

		// update is required
		update_ = script_->find_function( "update" );

		if ( init_ )
		{
			lua_params lp( par );
			lua_model lm( model );
			init_( &lm, &lp );
		}

		model.AddExternalResource( script_->script_file_ );
	}

	ScriptController::~ScriptController()
	{}

	void ScriptController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		SCONE_PROFILE_FUNCTION;

		if ( store_ )
		{
			lua_frame lf( frame );
			store_( &lf );
		}
	}

	bool ScriptController::ComputeControls( Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;
		
		lua_model lm( model );
		return update_( &lm );
	}

	String ScriptController::GetClassSignature() const
	{
		return "SC";
	}
}
