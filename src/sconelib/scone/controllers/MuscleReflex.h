/*
** MuscleReflex.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Reflex.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/core/Range.h"

namespace scone
{
	/// Reflex based on muscle length, muscle velocity, muscle force, or muscle spindle sensor.
	/// Must be part of ReflexController.
	class  MuscleReflex : public Reflex
	{
	public:
		MuscleReflex( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~MuscleReflex();

		virtual void ComputeControls( double timestamp ) override;

		/// Name of the source muscle (without side), leave empty for monosynaptic reflexes (default).
		String source;

		/// Constant actuation added to reflex output; default = 0.
		Real C0;

		/// Force feedback gain, based on nomralized muscle force (F / Fmax); default = 0.
		Real KF;
		/// Force feedback offset; default = 0.
		Real F0;
		/// Allow this reflex to be negative; default = 1.
		bool allow_neg_F;

		/// Length feedback gain, based on normalized CE length (L / Lopt); default = 0.
		Real KL;
		/// Length feedback offset; default = 1.
		Real K0;
		/// Allow this reflex to be negative; default = 1.
		bool allow_neg_L;

		/// Velocity feedback gain, based on normalized CE velocity ((L / Lopt) / s); default = 0.
		Real KV;
		/// Velocity feedback offset; default = 0.
		Real V0;
		/// Allow this reflex to be negative; default = 1.
		bool allow_neg_V;

		/// Spindle feedback gain, based on [Prochazka 1999], p.135; default = 0.
		Real KS;
		/// Spindle feedback offset; default = 0.
		Real S0;
		/// Allow this reflex to be negative; default = 1.
		bool allow_neg_S;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:

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
