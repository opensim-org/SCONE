/*
** JumpMeasure.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"

#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"

namespace scone
{
	/// Measure for jumping, supports both high-jump and long-jump. *Work in progress!*
	class JumpMeasure : public Measure
	{
	public:
		JumpMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~JumpMeasure();

		enum JumpType { NoJumpType, HighJump, LongJump };

		/// Type of jump
		JumpType jump_type;

		/// Relative COM height on which to terminate simulation
		Real termination_height;

		/// Terminate when peak height is detected
		bool terminate_on_peak;

		/// Time to prepare for jump
		Real prepare_time;

		/// Time to recover from jump
		Real recover_time;

		/// Body to measure, leave empty for COM (default).
		Body* target_body;

		/// Offset of body to measure from origin (NOT COM); default = [ 0 0 0 ].
		Vec3 offset;

		virtual double ComputeResult( Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		enum State { Prepare, Takeoff, Flight, Landing, Recover };

		double GetHighJumpResult( const Model& m );
		double GetLongJumpResult( const Model& m );
		static double GetLandingDist( const Vec3& pos, const Vec3& vel, double floor_height = 0.0 );
		Vec3 GetTargetPos( const Model& m ) const;

		State state;
		Vec3 init_com;
		Vec3 current_pos;
		double init_min_x;
		Vec3 prepare_com;
		Vec3 peak_com;
		Vec3 peak_com_vel;
		double peak_height;
		Vec3 recover_com;
		TimeInSeconds recover_start_time;
		Real recover_cop_dist = 1000.0;
		bool negate_result;
	};
}
