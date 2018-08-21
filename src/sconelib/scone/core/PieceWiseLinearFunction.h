#pragma once

#include "Function.h"
#include "scone/core/string_tools.h"

namespace scone
{
	class SCONE_API PieceWiseLinearFunction : public Function
	{
	public:
		PieceWiseLinearFunction( const PropNode& props, Params& par );
		virtual ~PieceWiseLinearFunction();

		virtual Real GetValue( Real x ) override;

		// a signature describing the function
		virtual String GetSignature() override;

	private:
		struct Impl;
		u_ptr< Impl > m_pImpl;
		bool flat_extrapolation;
	};
}
