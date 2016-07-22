#pragma once

#include "core.h"
#include <flut/math/quat.hpp>

namespace scone
{
	// import Quat from flut
	using Quat = flut::math::quat_< Real >;
	using Quatf = flut::math::quat_< float >;
	using Quatd = flut::math::quat_< double >;
}
