#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "scone/model/Joint.h"
#include "RangePenalty.h"

namespace scone
{
	// Class: JointLoadMeasure
	class JointLoadMeasure : public Measure
	{
	public:
		JointLoadMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~JointLoadMeasure() {}

		// Prop: min
		// Minimum

		// Prop: max
		// Maximum

		// Prop: abs_range_penalty
		// Absolute range penalty

		RangePenalty< Real > load_penalty;

		virtual double GetResult( Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		enum Method { NoMethod, JointReactionForce };
		int method;
		Real joint_load;
		Joint& joint;
	};
}
