#pragma once

#include "Reflex.h"

namespace scone
{
	class SCONE_API DofReflex : public Reflex
	{
	public:
		DofReflex( const PropNode& props, ParamSet& par, Model& model, const Locality& area );
		virtual ~DofReflex();

		virtual void ComputeControls( double timestamp );

		// Reflex parameters
		Real target_pos;
		Real target_vel;
		Real pos_gain;
		Real vel_gain;
		Real constant_u;

	private:
		SensorDelayAdapter& m_DelayedPos;
		SensorDelayAdapter& m_DelayedVel;
		SensorDelayAdapter& m_DelayedRootPos; // used for world coordinates, TODO: neater
		SensorDelayAdapter& m_DelayedRootVel; // used for world coordinates, TODO: neater
		bool m_bUseRoot;
	};
}
