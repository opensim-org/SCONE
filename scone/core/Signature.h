#pragma once
#include "core.h"
#include "PropNode.h"

namespace scone
{
	class CORE_API Signature
	{
	public:
		Signature( const PropNode& pn );
		virtual ~Signature();

		String GetSignature();

	protected:
		virtual String GetMainSignature() { return ""; }

	private:
		String signature_prefix;
		String signature_postfix;
	};
}
