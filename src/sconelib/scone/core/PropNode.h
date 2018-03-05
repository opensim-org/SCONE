#pragma once

#include "core.h"

#include <memory>
#include <vector>
#include <sstream>

#include "Exception.h"

#include "xo/container/prop_node.h"
#include "xo/container/prop_node_tools.h"
#include "xo/serialization/serialize.h"

namespace scone
{
	using PropNode = xo::prop_node;
	using xo::load_xml;
	using xo::load_file_with_include;
	using xo::save_xml;
	using xo::load_ini;
	using xo::save_ini;
}
