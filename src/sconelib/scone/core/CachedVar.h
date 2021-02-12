#pragma once

#include "types.h"
#include "xo/utility/cached_var.h"

namespace scone
{
	template< typename T >
	using CachedVar = xo::cached_var< T, TimeInSeconds >;
}
