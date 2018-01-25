#pragma once

#include "scone/core/types.h"
#include "scone/model/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/Vec3.h"
#include <random>

namespace scone
{
	class SCONE_API PerturbationController : public Controller
	{
	public:
		PerturbationController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~PerturbationController() {}

		String name;
		double force;
		double moment;
		TimeInSeconds interval;
		TimeInSeconds interval_min;
		TimeInSeconds interval_max;
		TimeInSeconds duration;
		TimeInSeconds start_time;
		TimeInSeconds end_time;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}
		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< TimeInSeconds > perturbation_times;

		unsigned int random_seed;
		Vec3 position_offset;
		Vec3 current_force;
		Vec3 current_moment;
		TimeInSeconds next_perturbation;
		Body& body_;
	};
}
