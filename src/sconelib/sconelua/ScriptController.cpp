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

namespace scone
{
	ScriptController::ScriptController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc )
	{
		lua_.script( "x = 1" );
	}

	bool ScriptController::ComputeControls( Model& model, double timestamp )
	{
		return false;
	}

	String ScriptController::GetClassSignature() const
	{
		return "S";
	}
}
