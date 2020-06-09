/*
** sconelib_config.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#ifdef SCONE_OPENSIM_3
#include "sconeopensim3/sconeopensim3.h"
#endif

#ifdef SCONE_OPENSIM_4
#include "sconeopensim3/sconeopensim4.h"
#endif

#ifdef SCONE_HYFYDY
#include "sconehfd/sconehfd.h"
#endif

#ifdef SCONE_LUA
#include "sconelua/sconelua.h"
#endif

#include "xo/serialization/serialize.h"
#include "xo/serialization/prop_node_serializer_zml.h"
#include "core/Settings.h"

namespace scone
{
	void Initialize()
	{
		// register .scone file format
		xo::register_file_extension< xo::prop_node_serializer_zml >( "scone" );

#ifdef SCONE_OPENSIM_3
		RegisterSconeOpenSim3();
#endif

#ifdef SCONE_OPENSIM_4
		RegisterSconeOpenSim4();
#endif

#ifdef SCONE_HYFYDY
		if ( GetSconeSetting<bool>( "hyfydy.enabled" ) )
			RegisterSconeHfd( GetSconeSetting<String>( "hyfydy.license" ).c_str() );
#endif

#ifdef SCONE_LUA
		RegisterSconeLua();
#endif
	}
}
