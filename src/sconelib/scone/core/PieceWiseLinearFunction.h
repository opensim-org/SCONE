#pragma once

#include "Function.h"
#include "OpenSim/Common/PiecewiseLinearFunction.h"
#include "PieceWiseFunction.h"
#include "scone/core/string_tools.h"

namespace scone
{
	class SCONE_API PieceWiseLinearFunction : public PieceWiseFunction
	{
	public:
		PieceWiseLinearFunction( bool flatExtrapolation );
		PieceWiseLinearFunction( const PropNode& props, Params& par );
		virtual ~PieceWiseLinearFunction();

		virtual Real GetValue( Real x ) override;
		OpenSim::PiecewiseLinearFunction& GetOsFunc() { return m_osFunc; }

		// a signature describing the function
		virtual String GetSignature() override { return stringf( "L%d", m_osFunc.getSize() ); }

	private:
		OpenSim::PiecewiseLinearFunction m_osFunc;
		bool flat_extrapolation;
	};
}
