#pragma once

#include "../core/core.h"
#include "../opt/ParamSet.h"
#include "../core/Named.h"
#include "cs.h"

namespace scone
{
	/// Parameterizable function
	class CS_API Function
	{
	public:
		Function() { };
		virtual ~Function() { };

		virtual Real GetValue( Real x ) = 0;
	};
}
