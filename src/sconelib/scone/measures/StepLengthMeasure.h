/*
** StepLengthMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once
#include "Measure.h"
#include "scone/core/Statistic.h"
#include "EffortMeasure.h"
#include "RangePenalty.h"

namespace scone
{
	/// Measure for locomotion at a predefined step length.
	/** Example:
	\verbatim
	StepLengthMeasure {
		step_length { min = 0.1 max = 0.4 abs_penalty = 1 }
	}
	\endverbatim
	*/
	class StepLengthMeasure : public Measure
	{
	public:
		StepLengthMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		/// Penalty for step length [m] when out of range.
		RangePenalty<double> step_length;

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

	protected:
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:

		Body* m_UpperBody;
		std::vector< Body* > m_BaseBodies;
		Real GetGaitDist( const Model &model );

		bool HasNewFootContact( const Model& model );
		std::vector< bool > m_PrevContactState;

		Real m_InitGaitDist;
		Real m_PrevGaitDist;
		Real m_PrevTime;
	};
}
