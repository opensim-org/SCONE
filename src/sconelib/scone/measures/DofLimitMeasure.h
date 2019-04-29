/*
** DofLimitMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "scone/core/Range.h"
#include "scone/core/Angle.h"
#include "scone/core/Statistic.h"

#include "scone/model/Dof.h"

namespace scone
{
	// Measure for penalizing when DOFs go out of a specific range.
	// Supports penalties based on DOF position, DOF velocity, and restitution force.
	// WARNING: deprecated, use DofMeasure instead.
	class DofLimitMeasure : public Measure
	{
	public:
		DofLimitMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~DofLimitMeasure();

		virtual double ComputeResult( Model& model ) override;

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;

		// min_deg - Minimum DoF value in degrees
		// max_deg - Minimum DoF value in degrees
		// min_deg_s - Minimum DoF velocity in degrees / s
		// max_deg_s - Maximum DoF velocity in degrees / s

	private:
		struct Limit
		{
			Limit( const PropNode& props, Model& model );
			Dof& dof;
			Dof* parent;
			Range< Degree > range;
			Range< Degree > velocity_range;
			Real squared_range_penalty;
			Real abs_range_penalty;
			Real squared_velocity_range_penalty;
			Real abs_velocity_range_penalty;
			Real squared_force_penalty;
			Real abs_force_penalty;
			Statistic<> penalty;
		};

		std::vector< Limit > m_Limits;
	};
}
