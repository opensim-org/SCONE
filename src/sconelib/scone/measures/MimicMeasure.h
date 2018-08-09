#pragma once

#include "Measure.h"
#include "../core/Statistic.h"

namespace scone
{
	class MimicMeasure : public Measure
	{
	public:
		MimicMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~MimicMeasure() { };

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;
		Storage<> storage_;
		xo::path motion_file_;
		Statistic<> result_;

	protected:
	private:
	};
}
