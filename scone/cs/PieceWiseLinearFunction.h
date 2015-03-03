#pragma once

#include "Function.h"
#include <OpenSim/OpenSim.h>
#include "PieceWiseFunction.h"

namespace scone
{
	class CS_API PieceWiseLinearFunction : public PieceWiseFunction
	{
	public:
		PieceWiseLinearFunction( bool flat_extrapolation );
		PieceWiseLinearFunction( const PropNode& props, opt::ParamSet& par );
		virtual ~PieceWiseLinearFunction();

		virtual Real GetValue( Real x ) override;

		virtual void AddPoint( Real x, Real y ) override;
		virtual Real GetX( size_t index ) override;
		virtual Real GetY( size_t index ) override;

		OpenSim::PiecewiseLinearFunction& GetOsFunc() { return m_osFunc; }

	private:
		OpenSim::PiecewiseLinearFunction m_osFunc;
		bool m_FlatExtrapolation;
	};
}
