#pragma once

#include "../core/core.h"

namespace scone
{
	class Function
	{
	public:
		Function() { };
		virtual ~Function() { };
		virtual Real GetValue( Real x ) = 0;
	};
}
