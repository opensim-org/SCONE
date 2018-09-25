#pragma once

#include "Measure.h"

namespace scone
{
	class SymmetricMeasure : public Measure
	{
	public:
		SymmetricMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~SymmetricMeasure() {}

		virtual double ComputeResult( Model& model ) override;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

	private:
		std::vector< MeasureUP > m_Measures;
	};
}
