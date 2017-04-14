#pragma once

#include "StateController.h"

namespace scone
{
	class SCONE_API SensorStateController : public StateController
	{
	public:
		SensorStateController( const PropNode& props, ParamSet& par, Model& model, const Area& area );

		virtual size_t GetStateCount() const override { return m_States.size(); }
		virtual const String& GetStateName( StateIndex i ) const override { return m_States[ i ].name; }

	protected:
		virtual StateIndex GetCurrentState( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

		struct SensorState
		{
			SensorState( const PropNode& pn, ParamSet& par );
			double GetDistance( Model& model, double timestamp ) const;
			String name;
			bool mirrored;
			double leg_load[ 2 ];
			double sag_pos[ 2 ];
		};

		std::vector< SensorState > m_States;
		StateIndex m_CurrentState;
	};
}
