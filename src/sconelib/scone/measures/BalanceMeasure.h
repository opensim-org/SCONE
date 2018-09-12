#pragma once

#include "Measure.h"

namespace scone
{
	/// Measure that checks for balance, i.e. if a specific vertical COP position is maintained.
	class BalanceMeasure : public Measure
	{
	public:
		BalanceMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~BalanceMeasure() { };

		/// Relative COM height (factor of initial COM height) at which to stop the simulation; default = 0.5.
		double termination_height;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		double m_InitialHeight;
	};
}
