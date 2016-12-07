#pragma once

#include "core.h"
#include "String.h"

#include <memory>
#include <vector>
#include <sstream>

#include "Exception.h"

#include "flut/prop_node.hpp"
#include "flut/prop_node_tools.hpp"

namespace scone
{
	using PropNode = flut::prop_node;
	using flut::load_xml;
	using flut::load_file_with_include;
	using flut::save_xml;
	using flut::load_ini;
	using flut::save_ini;
}
