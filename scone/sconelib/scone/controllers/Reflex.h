#pragma once

#include "scone/model/Model.h"
#include "scone/optimization/ParamSet.h"
#include "scone/core/PropNode.h"

namespace scone
{
	class SCONE_API Reflex
	{
	public:
		Reflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area );
		virtual ~Reflex();

		virtual void ComputeControls( double timestamp );
		TimeInSeconds delay;

	protected:
		/// clamp control value between min_control_value and max_control_value and add to target actuator
		Real AddTargetControlValue( Real u );
		sim::Actuator& m_Target;
		static String GetReflexName( const String& target, const String& source );

	private:
		Real min_control_value;
		Real max_control_value;
	};
}
