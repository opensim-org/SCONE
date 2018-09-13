#pragma once

#include "scone/core/platform.h"
#include "scone/optimization/Params.h"
#include "scone/core/HasName.h"

namespace scone
{
	/// Abstract parameterizable function, see subclasses for actual implementations.
	class SCONE_API Function
	{
	public:
		Function() { };
		virtual ~Function() { };

		virtual Real GetValue( Real x ) = 0;
		virtual String GetSignature() { return "UnknownFunction"; }
	};
}
