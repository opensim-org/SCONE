/*
** GaitMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Measure.h"
#include "scone/core/Statistic.h"
#include "EffortMeasure.h"
#include "DofLimitMeasure.h"

namespace scone
{
	/// Measure for efficient locomotion at a predefined speed.
	class GaitMeasure : public Measure
	{
	public:
		GaitMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Relative COM height (wrt initial position) at which to stop the simulation; default = 0.5.
		Real termination_height;

		/// Minimum velocity [m/s]; default = 0 m/s.
		Real min_velocity;

		/// Maximum velocity [m/s]; default = 299792458 m/s.
		Real max_velocity;

		/// Load threshold for step detection; default = 0.1.
		Real load_threshold;

		/// Minimum duration [s] of a step, used for step detection; default = 0.1.
		Real min_step_duration;

		/// Number of initial steps of which the velocity is disregarded in the final measure; default = 2.
		int initiation_steps;

		/// Name of the upper body, used for gait detection; default = "torso"
		String upper_body;

		/// Name of the base bodies (i.e. feet), used for gait detection; default = "toes_l toes_r"
		String base_bodies;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		void AddStep( const Model &model, double timestamp );
		virtual double ComputeResult( const Model& model ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		struct Step {
			TimeInSeconds time;
			Real length;
		};
		std::vector< Step > steps_;

		Body* m_UpperBody;
		std::vector< Body* > m_BaseBodies;
		Real GetGaitDist( const Model &model );

		bool HasNewFootContact( const Model& model );
		std::vector< bool > m_PrevContactState;

		Vec3 m_InitialComPos;
		Real m_InitGaitDist;
		Real m_PrevGaitDist;

		PropNode m_Report;
	};
}
