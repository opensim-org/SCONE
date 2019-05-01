/*
** sconelib_config.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/Factories.h"

#ifdef SCONE_OPENSIM_3
#include "sconeopensim3/ModelOpenSim3.h"
#endif

#ifdef SCONE_OPENSIM_4
#include "sconeopensim4/ModelOpenSim4.h"
#endif

#ifdef SCONE_LUA
#include "sconelua/ScriptController.h"
#endif

namespace scone
{
	void RegisterModels()
	{
#ifdef SCONE_OPENSIM_3
	GetModelFactory().register_type< ModelOpenSim3 >( "Simbody" );
	GetModelFactory().register_type< ModelOpenSim3 >( "OpenSim3Model" );
	GetModelFactory().register_type< ModelOpenSim3 >( "OpenSimModel" );
#endif
#ifdef SCONE_OPENSIM_4
	GetModelFactory().register_type< ModelOpenSim4 >( "OpenSim4Model" );
#endif
#ifdef SCONE_LUA
	GetControllerFactory().register_type< ScriptController >( "ScriptController" );
#endif
	}
}
