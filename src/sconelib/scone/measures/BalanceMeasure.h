#pragma once

#include "Measure.h"

namespace scone
{
	class BalanceMeasure : public Measure
	{
	public:
		BalanceMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~BalanceMeasure() { };

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		double m_InitialHeight;
		double termination_height;
	};
}
