#pragma once

#include "Function.h"
#include <OpenSim/OpenSim.h>

namespace scone
{
	class PieceWiseLinearFunction : public Function
	{
	public:
		PieceWiseLinearFunction( bool flat_extrapolation );
		virtual ~PieceWiseLinearFunction();

		virtual Real GetValue( Real x ) override;

		OpenSim::PiecewiseLinearFunction& GetOsFunc() { return m_osFunc; }

	private:
		OpenSim::PiecewiseLinearFunction m_osFunc;
		bool m_FlatExtrapolation;
	};
}
