#pragma once

#include "Reflex.h"
#include "xo/numerical/filter.h"

namespace scone
{
	class SCONE_API DofReflex : public Reflex
	{
	public:
		DofReflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~DofReflex();

		virtual void ComputeControls( double timestamp );

		// Reflex parameters
		Real target_pos;
		Real target_vel;
		Real pos_gain;
		Real vel_gain;
		Real constant_u;
		Real filter_cutoff_frequency;
		int condition; // 1 = pos, -1 = neg, 0 = always


		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		String name;
		Real u_p;
		Real u_d;
		SensorDelayAdapter& m_DelayedPos;
		SensorDelayAdapter& m_DelayedVel;
		SensorDelayAdapter& m_DelayedRootPos; // used for world coordinates, TODO: neater
		SensorDelayAdapter& m_DelayedRootVel; // used for world coordinates, TODO: neater
		bool m_bUseRoot;
		xo::iir_filter< double, 2 > m_Filter;
	};
}
