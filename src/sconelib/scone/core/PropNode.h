/*
** PropNode.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"

#include "xo/container/prop_node.h"
#include "xo/container/prop_node_tools.h"
#include "xo/serialization/serialize.h"

namespace scone
{
	using PropNode = xo::prop_node;
	using xo::load_file;
	using xo::load_file_with_include;
	using xo::save_file;
}
