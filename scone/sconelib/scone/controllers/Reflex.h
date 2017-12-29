#pragma once

#include "scone/model/Model.h"
#include "scone/optimization/Params.h"
#include "scone/core/PropNode.h"
#include "scone/core/math.h"

namespace scone
{
	class SCONE_API Reflex : public HasData
	{
	public:
		Reflex( const PropNode& props, Params& par, Model& model, const Locality& area );
		Reflex( Actuator& target, TimeInSeconds d, Real min_val = REAL_LOWEST, Real max_val = REAL_MAX );
		virtual ~Reflex();

		virtual void ComputeControls( double timestamp );
		TimeInSeconds delay;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}

	protected:
		/// clamp control value between min_control_value and max_control_value and add to target actuator
		Real AddTargetControlValue( Real u );
		Actuator& m_Target;
		static String GetReflexName( const String& target, const String& source );
		static String GetParName( const PropNode& props );

	private:
		Real min_control_value;
		Real max_control_value;
	};
}
