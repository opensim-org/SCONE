#pragma once

#include "Reflex.h"
#include "flut/filter.hpp"

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
		flut::iir_filter< double, 2 > m_Filter;
	};
}
