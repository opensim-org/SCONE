#pragma once

#include "scone/core/core.h"
#include "scone/opt/ParamSet.h"
#include "scone/core/HasName.h"

namespace scone
{
	/// Parameterizable function
	class SCONE_API Function
	{
	public:
		Function() { };
		virtual ~Function() { };

		virtual Real GetValue( Real x ) = 0;

		// a signature describing the function
		virtual String GetSignature() { return "UnknownFunction"; }
	};
}
