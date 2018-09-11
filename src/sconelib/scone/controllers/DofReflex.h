#pragma once

#include "Reflex.h"
#include "xo/numerical/filter.h"

namespace scone
{
	class DofReflex : public Reflex
	{
	public:
		DofReflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~DofReflex();

		virtual void ComputeControls( double timestamp );

		/// Reflex sensor dof
		String source;
		/// Target position for sensor dof
		Real P0; 
		/// Target velocity for sensor dof
		Real V0; 
		/// Position gain
		Real KP; 
		/// Velocity gain
		Real KV;
		/// Constant actuation
		Real C0; 
		/// Cut-off frequency of optional low-pass filter, no filtering if zero
		Real filter_cutoff_frequency; 
		/// Apply this reflex only depending on the sign of the result: 1 = pos, -1 = neg, 0 = always
		int condition; 

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		String name;
		Real u_p;
		Real u_d;
		Dof& m_SourceDof;
		Dof* m_SourceParentDof;
		SensorDelayAdapter* m_pTargetPosSource;
		SensorDelayAdapter& m_DelayedPos;
		SensorDelayAdapter& m_DelayedVel;
		xo::iir_filter< double, 2 > m_Filter;
	};
}
