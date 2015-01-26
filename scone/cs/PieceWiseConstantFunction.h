#pragma once

#include "Function.h"
#include <OpenSim/OpenSim.h>
#include <OpenSim/Common/PiecewiseConstantFunction.h>

namespace scone
{
	class PieceWiseConstantFunction : public Function
	{
	public:
		PieceWiseConstantFunction();
		virtual ~PieceWiseConstantFunction();

		virtual Real GetValue( Real x ) override;

		OpenSim::PiecewiseConstantFunction& GetOsFunc() { return m_osFunc; }

	private:
		OpenSim::PiecewiseConstantFunction m_osFunc;
	};
}
