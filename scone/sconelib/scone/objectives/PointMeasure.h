#pragma once

#include "Measure.h"
#include "scone/model/Body.h"
#include "scone/core/Range.h"
#include "scone/core/Statistic.h"

namespace scone
{
	class PointMeasure : public Measure
	{
	public:
		PointMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~PointMeasure() { };

		virtual UpdateResult UpdateAnalysis( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame ) override;

	private:
		String body;
		Vec3 offset;
		Vec3 axes_to_measure;
		bool relative_to_model_com;

		Range< Real > range;
		Range< Real > vel_range;
		Range< Real > acc_range;
		Real squared_range_penalty;
		Real abs_range_penalty;
		Real squared_velocity_range_penalty;
		Real abs_velocity_range_penalty;
		Real squared_acceleration_range_penalty;
		Real abs_acceleration_range_penalty;
		Statistic<> penalty;

		Body* m_pTargetBody; // non-owning pointer
	};
}
