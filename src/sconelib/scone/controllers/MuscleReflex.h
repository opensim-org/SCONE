#pragma once

#include "Reflex.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/Range.h"

namespace scone
{
	class SCONE_API MuscleReflex : public Reflex
	{
	public:
		MuscleReflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~MuscleReflex();

		virtual void ComputeControls( double timestamp ) override;

		Real KL; ///< Length feedback gain
		Real K0; ///< Length feedback offset
		bool allow_neg_L; ///< Allow this reflex to be negative

		Real KV; ///< Velocity feedback gain
		Real V0; ///< Velocity feedback offset
		bool allow_neg_V; ///< Allow this reflex to be negative

		Real KF; ///< Force feedback gain
		Real F0; ///< Force feedback offset
		bool allow_neg_F; ///< Allow this reflex to be negative

		Real KS; ///< Muscle spindle feedback gain
		Real S0; ///< Muscle spindle feedback offset
		bool allow_neg_S; ///< Allow this reflex to be negative

		Real C0; ///< Constant actuation added to reflex results

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
