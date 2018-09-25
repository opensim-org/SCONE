#pragma once

#include "Measure.h"
#include "RangePenalty.h"
#include "scone/core/Angle.h"
#include "scone/model/Dof.h"

namespace scone
{
	/// Measure for penalizing when DOFs go out of a specific range.
	/// Supports penalties based on DOF position, DOF velocity, and restitution force.
	class DofMeasure : public Measure
	{
	public:
		DofMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~DofMeasure() {}
		virtual double ComputeResult( Model& model ) override;

	protected:
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

		/// Dof to which to apply the penalty to.
		Dof& dof;

		/// Optional parent dof which will be added to the dof value.
		Dof* parent;

		/// Penalty for when the DOF position [deg] is out of range.
		RangePenalty< Degree > position;

		/// Penalty for when the DOF velocity [deg/s] is out of range.
		RangePenalty< Degree > velocity;

		/// Penalty for when the DOF limit force [N] is out of range.
		RangePenalty< double > force;

	private:
		int range_count;
	};
}
