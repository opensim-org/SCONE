#pragma once

#include "cs_fwd.h"
#include "scone/sim/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/opt/ParamSet.h"
#include "scone/core/Vec3.h"

namespace scone
{
	namespace cs
	{
		class SCONE_API PerturbationController : public sim::Controller
		{
		public:
			PerturbationController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~PerturbationController() {}

			double force;
			TimeInSeconds interval;
			TimeInSeconds duration;
			TimeInSeconds start;

			virtual void StoreData( Storage<Real>::Frame& frame ) override;
			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;

		protected:
			virtual String GetClassSignature() const override;

		private:
			Vec3 current_force;
			sim::Body& force_body;
		};
	}
}
