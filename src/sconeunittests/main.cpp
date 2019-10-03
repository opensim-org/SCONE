/*
** main.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "scone/sconelib_config.h"
#include "xo/serialization/serialize.h"
#include "xo/system/log_sink.h"
#include "xo/system/test_case.h"
#include "xo/serialization/prop_node_serializer_zml.h"

int main( int argc, const char* argv[] )
{
	xo::log::console_sink sink( xo::log::info_level );
	xo::register_serializer< xo::prop_node_serializer_zml >( "scone" );
	scone::RegisterModels();

	return xo::test::run_all_test_cases();
}
