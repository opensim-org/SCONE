/*
** DofReflex.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Reflex.h"
#include "xo/numerical/filter.h"

namespace scone
{
	/// Reflex based on the value of a specific DOF.
	/// Must be part of ReflexController.
	class DofReflex : public Reflex
	{
	public:
		DofReflex( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~DofReflex();

		virtual void ComputeControls( double timestamp ) override;

		/// Name of the DOF that is the source of this Reflex, append with _o for DOF on opposite side.
		String source;

		/// Target position [rad or m] for sensor DOF; default = 0.
		Real P0; 

		/// Target velocity [rad or m] for sensor DOF; default = 0.
		Real V0; 

		/// Position feedback gain; default = 0.
		Real KP; 

		/// Velocity feedback gain; default = 0.
		Real KV;

		/// Constant actuation added to the reflex; default = 0.
		Real C0;

		/// Cut-off frequency of optional low-pass filter, no filtering if zero; default = 0.
		Real filter_cutoff_frequency; 

		/// Apply this reflex only depending on the sign of the result: 1 = pos, -1 = neg, 0 = always.
		int condition; 

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		Real u_p;
		Real u_v;
		Dof& m_SourceDof;
		Dof* m_SourceParentDof;
		SensorDelayAdapter* m_pTargetPosSource;
		SensorDelayAdapter& m_DelayedPos;
		SensorDelayAdapter& m_DelayedVel;
		xo::iir_filter< double, 2 > m_Filter;
	};
}
