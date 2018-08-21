#pragma once

#include "Function.h"
#include "scone/core/string_tools.h"
#include "PropNode.h"

namespace scone
{
	class SCONE_API PieceWiseConstantFunction : public Function
	{
	public:
		PieceWiseConstantFunction( const PropNode& props, Params& par );
		virtual ~PieceWiseConstantFunction();

		virtual Real GetValue( Real x ) override;

		// a signature describing the function
		virtual String GetSignature() override;

	private:
		struct Impl;
		u_ptr< Impl > m_pImpl;
	};
}
