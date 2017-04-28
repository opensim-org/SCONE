#pragma once

#include "scone/core/types.h"
#include "scone/model/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/ParamSet.h"
#include "scone/core/Vec3.h"

namespace scone
{
	class SCONE_API ExternalBalanceController : public Controller
	{
	public:
		ExternalBalanceController( const PropNode& props, ParamSet& par, Model& model, const Locality& target_area );

		String name;
		double angular_velocity_gain;
		double angular_velocity_target;
		double angular_position_gain;
		double angular_position_target;

		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		Index state_idx;
		Body& force_body;
	};
}
