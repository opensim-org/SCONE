#pragma once

#include "Measure.h"

#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"

namespace scone
{
	class JumpMeasure : public Measure
	{
	public:
		JumpMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
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

		virtual double GetResult( Model& model ) override;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

	private:
		enum State { Prepare, Takeoff, Flight, Landing, Recover };

		double GetHighJumpResult( const Model& m );
		double GetLongJumpResult( const Model& m );
		static double GetLandingDist( const Vec3& pos, const Vec3& vel, double floor_height = 0.0 );
		Vec3 GetTargetPos( const Model& m ) const;

		State state;
		Body* target_body;
		Vec3 init_com;
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
