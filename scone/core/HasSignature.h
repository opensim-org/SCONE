#pragma once
#include "core.h"
#include "PropNode.h"

namespace scone
{
	class CORE_API HasSignature
	{
	public:
		HasSignature( const PropNode& pn );
		virtual ~HasSignature();

		String GetSignature();

	protected:
		virtual String GetMainSignature() { return ""; }

	private:
		String signature_prefix;
		String signature_postfix;
	};
}
