#pragma once

#include "core.h"
#include "String.h"
#include "PropNode.h"

namespace scone
{
	class SCONE_API HasSignature
	{
	public:
		HasSignature( const PropNode& pn );
		virtual ~HasSignature();

		String GetSignature() const;

	protected:
		virtual String GetClassSignature() const { return ""; }

	private:
		String signature_prefix;
		String signature_postfix;
	};
}			
