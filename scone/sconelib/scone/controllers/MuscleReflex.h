#pragma once

#include "Reflex.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/Range.h"

namespace scone
{
	class SCONE_API MuscleReflex : public Reflex
	{
	public:
		MuscleReflex( const PropNode& props, ParamSet& par, Model& model, const Area& area );
		virtual ~MuscleReflex();

		virtual void ComputeControls( double timestamp ) override;

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

		Real u_constant;

	private:
		SensorDelayAdapter* m_pForceSensor;
		SensorDelayAdapter* m_pLengthSensor;
		SensorDelayAdapter* m_pVelocitySensor;
	};
}
