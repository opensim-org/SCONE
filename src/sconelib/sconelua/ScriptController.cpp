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
#include "LuaScript.h"
#include "scone_lua_api.h"

namespace scone
{
	ScriptController::ScriptController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc ),
	script_( new LuaScript( props, par, model ) )
	{
		init_ = script_->GetFunction( "init" );
		update_ = script_->GetFunction( "update" );

		lua_params lp( par );
		lua_model lm( model );
		init_( &lm, &lp );

		model.AddExternalResource( script_->GetFile() );
	}

	ScriptController::~ScriptController()
	{}

	bool ScriptController::ComputeControls( Model& model, double timestamp )
	{
		lua_model lm( model );
		update_( &lm );
		return false;
	}

	String ScriptController::GetClassSignature() const
	{
		return "SC";
	}
}
