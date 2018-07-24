#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "scone/model/Joint.h"
#include "RangePenalty.h"

namespace scone
{
	class SCONE_API JointLoadMeasure : public Measure
	{
	public:
		JointLoadMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~JointLoadMeasure() {}

		enum Method { NoMethod, JointReactionForce };

		virtual double GetResult( Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		int method;
		Real joint_load;
		RangePenalty< Real > load_penalty;
		Joint& joint;
	};
}
