/*
** sconelib_config.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#ifdef SCONE_OPENSIM_3
#include "sconeopensim3/sconeopensim3.h"
#define SCONE_OPENSIM_3_ENABLED 1
#else
#define SCONE_OPENSIM_3_ENABLED 0
#endif

#ifdef SCONE_OPENSIM_4
#include "sconeopensim4/sconeopensim4.h"
#define SCONE_OPENSIM_4_ENABLED 1
#else
#define SCONE_OPENSIM_4_ENABLED 0
#endif

#ifdef SCONE_HYFYDY
#include "sconehfd/sconehfd.h"
#define SCONE_HYFYDY_ENABLED 1
#else
#define SCONE_HYFYDY_ENABLED 0
#endif

#ifdef SCONE_LUA
#include "sconelua/sconelua.h"
#define SCONE_LUA_ENABLED 1
#else
#define SCONE_LUA_ENABLED 0
#endif

#include "xo/serialization/serialize.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "core/Settings.h"

namespace scone
{
	inline void Initialize()
	{
		// register .scone file format
		xo::register_file_extension< xo::prop_node_serializer_zml >( "scone" );

#if SCONE_OPENSIM_3_ENABLED
		RegisterSconeOpenSim3();
#endif

#if SCONE_OPENSIM_4_ENABLED
		RegisterSconeOpenSim4();
#endif

#if SCONE_HYFYDY_ENABLED
		if ( GetSconeSetting<bool>( "hyfydy.enabled" ) )
			RegisterSconeHfd( GetSconeSetting<String>( "hyfydy.license" ) );
#endif

#if SCONE_LUA_ENABLED
		RegisterSconeLua();
#endif
	}
}
