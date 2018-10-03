/*
** Reflex.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/model/Model.h"
#include "scone/optimization/Params.h"
#include "scone/core/PropNode.h"
#include "scone/core/math.h"

namespace scone
{
	/// Base class for reflexes, requires use of ReflexController.
	class Reflex : public HasData
	{
	public:
		Reflex( const PropNode& props, Params& par, Model& model, const Location& loc );
		Reflex( Actuator& target, TimeInSeconds d, Real min_val = REAL_LOWEST, Real max_val = REAL_MAX );
		virtual ~Reflex();

		/// Name of the target muscle.
		String target;

		/// Minimum output for this reflex; default = 0.
		Real min_control_value;

		/// Maximum output for this reflex; default = 0.
		Real max_control_value;

		/// Neuromuscular delay [s] used for this reflex; default = 0.
		TimeInSeconds delay;

		virtual void ComputeControls( double timestamp );
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}

	protected:
		/// clamp control value between min_control_value and max_control_value and add to target actuator
		Real AddTargetControlValue( Real u );
		Actuator& actuator_;
		static String GetReflexName( const String& target, const String& source );
		static String GetParName( const PropNode& props );
	};
}
