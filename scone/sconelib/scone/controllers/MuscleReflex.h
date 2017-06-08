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

		String name;

		// Reflex parameters
		Real length_gain;
		Real length_ofs;
		bool length_allow_negative;

		Real velocity_gain;
		Real velocity_ofs;
		bool velocity_allow_negative;

		Real force_gain;
		Real force_ofs;
		bool force_allow_negative;

		Real spindle_gain;
		Real spindle_ofs;
		bool spinde_allow_negative;

		Real u_constant;

		Real u_l = 0;
		Real u_v = 0;
		Real u_f = 0;
		Real u_s = 0;
		Real u_total = 0;

		virtual void StoreData( Storage<Real>::Frame& frame ) override;

	private:
		SensorDelayAdapter* m_pForceSensor;
		SensorDelayAdapter* m_pLengthSensor;
		SensorDelayAdapter* m_pVelocitySensor;
		SensorDelayAdapter* m_pSpindleSensor;
	};
}
