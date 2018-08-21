#pragma once

#include "platform.h"
#include "xo/geometry/quat.h"

namespace scone
{
	// import Quat from xo
	using Quat = xo::quat_< Real >;
	using Quatf = xo::quat_< float >;
	using Quatd = xo::quat_< double >;
}
