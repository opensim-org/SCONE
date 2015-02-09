#pragma once

#include "Link.h"
#include "Types.h"

namespace scone
{
	namespace sim
	{
		class Leg
		{
		public:
			enum State { UnknownState = 0, StanceState, LiftoffState, SwingState, LandingState };

			Leg( Link& proximal, Link& foot, Side side = NoSide, size_t rank = 1 ) : m_pProximal( &proximal ), m_pFoot( &foot ), m_Side( side ), m_Rank( rank ), m_State( UnknownState ), m_StartTime( 0.0 ) { };

			Link& GetFootLink() { return *m_pFoot; }
			Link& GetUpperLink() { return *m_pProximal; }

			State GetState() { return m_State; }
			double GetStateStartTime() { return m_StartTime; }
			Side GetSide() { return m_Side; }

			void UpdateState();

		private:
			State m_State;
			double m_StartTime;

			Side m_Side;
			size_t m_Rank;
			Link* m_pFoot;
			Link* m_pProximal;
		};
	}
}
