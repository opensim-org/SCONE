#pragma once

#include "Function.h"
#include <OpenSim/OpenSim.h>
#include <OpenSim/Common/PiecewiseConstantFunction.h>
#include "PieceWiseFunction.h"

namespace scone
{
	class CS_API PieceWiseConstantFunction : public PieceWiseFunction
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
		virtual String GetSignature() override { return GetStringF( "PWC%d", m_osFunc.getSize() ); }

	private:
		OpenSim::PiecewiseConstantFunction m_osFunc;
	};
}
