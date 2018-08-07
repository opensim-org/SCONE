#pragma once

#include "scone/core/types.h"
#include "scone/model/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/Vec3.h"

namespace scone
{
	class SCONE_API ExternalBalanceController : public Controller
	{
	public:
		ExternalBalanceController( const PropNode& props, Params& par, Model& model, const Locality& target_area );

		String name;
		double angular_velocity_gain;
		double angular_velocity_target;
		double angular_position_gain;
		double angular_position_target;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		index_t state_idx;
		Body& force_body;
	};
}
