#pragma once

#include "Function.h"
#include <OpenSim/OpenSim.h>
#include "PieceWiseFunction.h"

namespace scone
{
	class CS_API PieceWiseLinearFunction : public PieceWiseFunction
	{
	public:
		PieceWiseLinearFunction( bool flatExtrapolation );
		PieceWiseLinearFunction( const PropNode& props, opt::ParamSet& par );
		virtual ~PieceWiseLinearFunction();

		virtual Real GetValue( Real x ) override;

		virtual void AddPoint( Real x, Real y ) override;
		virtual Real GetX( size_t index ) override;
		virtual Real GetY( size_t index ) override;

		OpenSim::PiecewiseLinearFunction& GetOsFunc() { return m_osFunc; }

		// a signature describing the function
		virtual String GetSignature() override { return GetStringF( "L%d", m_osFunc.getSize() ); }

	private:
		OpenSim::PiecewiseLinearFunction m_osFunc;
		bool flat_extrapolation;
	};
}
