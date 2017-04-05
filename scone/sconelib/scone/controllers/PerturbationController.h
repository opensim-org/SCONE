#pragma once

#include "scone/core/types.h"
#include "scone/model/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/ParamSet.h"
#include "scone/core/Vec3.h"

namespace scone
{
	class SCONE_API PerturbationController : public sim::Controller
	{
	public:
		PerturbationController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
		virtual ~PerturbationController() {}

		String name;
		double force;
		TimeInSeconds interval;
		TimeInSeconds duration;
		TimeInSeconds start_time;

		virtual void StoreData( Storage<Real>::Frame& frame ) override;
		virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Index state_idx;
		Vec3 current_force;
		sim::Body& force_body;
	};
}