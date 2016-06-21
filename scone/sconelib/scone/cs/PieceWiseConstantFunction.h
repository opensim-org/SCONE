#pragma once

#include "Function.h"
#include <OpenSim/Common/PiecewiseConstantFunction.h>
#include "PieceWiseFunction.h"
#include "scone/core/string_tools.h"

namespace scone
{
	class SCONE_API PieceWiseConstantFunction : public PieceWiseFunction
	{
	public:
		PieceWiseConstantFunction();
		PieceWiseConstantFunction( const PropNode& props, opt::ParamSet& par );
		virtual ~PieceWiseConstantFunction();

		virtual Real GetValue( Real x ) override;

		/// Piece wise functions
		virtual void AddPoint( Real x, Real y ) override;
		virtual Real GetX( size_t index ) override;
		virtual Real GetY( size_t index ) override;

		OpenSim::PiecewiseConstantFunction& GetOsFunc() { return m_osFunc; }

		// a signature describing the function
		virtual String GetSignature() override { return stringf( "C%d", m_osFunc.getSize() ); }

	private:
		OpenSim::PiecewiseConstantFunction m_osFunc;
	};
}
