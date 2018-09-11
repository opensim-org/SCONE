#pragma once

#include "Reflex.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/Range.h"

namespace scone
{
	// class: MuscleReflex
	class  MuscleReflex : public Reflex
	{
	public:
		MuscleReflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~MuscleReflex();

		virtual void ComputeControls( double timestamp ) override;

		// props: Muscle length reflexes
		// KL - Length feedback gain. Default = 0.
		// K0 - Length feedback offset. Default = 1.
		// allow_neg_L - Allow this reflex to be negative. Default = 1.
		Real KL;
		Real K0;
		bool allow_neg_L;

		// props: Muscle velocity reflexes
		// KV - Velocity feedback gain. Default = 0.
		// V0 - Velocity feedback offset. Default = 0.
		// allow_neg_V - Allow this reflex to be negative. Default = 1.
		Real KV;
		Real V0;
		bool allow_neg_V;

		// props: Muscle force reflexes
		// KF - Velocity feedback gain. Default = 0.
		// F0 - Velocity feedback offset. Default = 0.
		// allow_neg_F - Allow this reflex to be negative. Default = 1.
		Real KF;
		Real F0;
		bool allow_neg_F;

		// props: Muscle spindle reflexes
		// KS - Spindle feedback gain. Default = 0.
		// S0 - Spindle feedback offset. Default = 0.
		// allow_neg_S - Allow this reflex to be negative. Default = 1.
		Real KS;
		Real S0;
		bool allow_neg_S;

		// prop: C0
		// Constant actuation added to reflex output
		Real C0;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:

		String name;
		Real u_l = 0;
		Real u_v = 0;
		Real u_f = 0;
		Real u_s = 0;
		Real u_total = 0;

	private:
		SensorDelayAdapter* m_pForceSensor;
		SensorDelayAdapter* m_pLengthSensor;
		SensorDelayAdapter* m_pVelocitySensor;
		SensorDelayAdapter* m_pSpindleSensor;
	};
}
