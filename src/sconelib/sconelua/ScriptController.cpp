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

namespace scone
{
	ScriptController::ScriptController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc ),
	script_( new lua_script( props, par, model ) )
	{
		init_ = script_->find_function( "init" );
		update_ = script_->find_function( "update" );

		try
		{
			lua_params lp( par );
			lua_model lm( model );
			init_( &lm, &lp );
		}
		catch ( const std::exception& e )
		{
			SCONE_ERROR( "Error in " + script_->script_file_.str() + " while calling init(): " + e.what() );
		}

		model.AddExternalResource( script_->script_file_ );
	}

	ScriptController::~ScriptController()
	{}

	bool ScriptController::ComputeControls( Model& model, double timestamp )
	{
		try
		{
			lua_model lm( model );
			update_( &lm );
		}
		catch ( const std::exception& e )
		{
			log::error( "Error in ", script_->script_file_.str(), " while calling update(): ", e.what() );
			return true;
		}

		return false;
	}

	String ScriptController::GetClassSignature() const
	{
		return "SC";
	}
}
