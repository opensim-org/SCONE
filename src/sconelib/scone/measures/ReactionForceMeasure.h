/*
** ReactionForceMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "RangePenalty.h"

namespace scone
{
	/// Measure that penalizes ground reaction forces above a certain threshold.
	/// See RangePenalty for details on how to set range, abs_penalty and squared_penalty.
	class ReactionForceMeasure : public Measure, public RangePenalty<Real>
	{
	public:
		ReactionForceMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		virtual double ComputeResult( const Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;

	protected:
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
	};
}
