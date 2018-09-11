#pragma once

#include "StateController.h"

namespace scone
{
	class SensorStateController : public StateController
	{
	public:
		SensorStateController( const PropNode& props, Params& par, Model& model, const Locality& area );

		virtual size_t GetStateCount() const override { return m_States.size(); }
		virtual const String& GetStateName( StateIndex i ) const override { return m_States[ i ].name; }
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual StateIndex GetCurrentState( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;

		struct SensorState
		{
			SensorState( const PropNode& pn, Params& par, const Locality& a );
			double GetDistance( Model& model, double timestamp );
			String name;
			bool mirrored;
			double load_delta;
			double sag_delta;
			double ld = 0, sd = 0;
		};

		bool create_mirrored_state;
		bool mirrored;
		std::vector< SensorState > m_States;
		std::vector< double > m_StateDist;
	};
}
