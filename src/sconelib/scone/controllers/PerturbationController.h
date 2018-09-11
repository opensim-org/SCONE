#pragma once

#include "scone/core/types.h"
#include "scone/model/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/Vec3.h"
#include <random>

namespace scone
{
	// Class: PerturbationController
	class PerturbationController : public Controller
	{
	public:
		PerturbationController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~PerturbationController() {}

		// Prop: force
		// Perturbation force to apply. Default = [ 0 0 0 ]
		Vec3 force;

		// Prop: moment
		// Perturbation moment to apply. Default = [ 0 0 0 ]
		Vec3 moment;

		TimeInSeconds interval; ///< Time between two perturbations (fixed)
		TimeInSeconds interval_min; ///< Minimum time between two perturbations (random)
		TimeInSeconds interval_max; ///< Maximum time between two perturbations (random)
		TimeInSeconds duration; ///< Perturbation duration

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}
		virtual bool ComputeControls( Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< TimeInSeconds > perturbation_times;

		bool active_;
		unsigned int random_seed;
		Vec3 position_offset;
		Vec3 current_force;
		Vec3 current_moment;
		TimeInSeconds next_perturbation;
		Body& body_;
	};
}
