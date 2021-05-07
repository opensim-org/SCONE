/*
** JointLoadMeasure.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "scone/core/Statistic.h"
#include "scone/model/Joint.h"
#include "RangePenalty.h"

namespace scone
{
	/// Measure for penalizing joint load, if above a specific threshold.
	/// See RangePenalty for details on how to set range, abs_penalty and squared_penalty.
	class JointLoadMeasure : public Measure, public RangePenalty<Real>
	{
	public:
		JointLoadMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		virtual double ComputeResult( const Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		enum Method { NoMethod, JointReactionForce };
		int method;
		Real joint_load;
		const Joint& joint;
	};
}
