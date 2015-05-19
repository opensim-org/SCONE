#include "stdafx.h"

#include "Leg.h"
#include "Link.h"
#include "Body.h"
#include "Joint.h"
#include "../core/Profiler.h"

namespace scone
{
	namespace sim
	{
		Leg::Leg( const Link& proximal, const Link& foot, size_t index, Side side, size_t rank ) :
		m_Upper( proximal ),
		m_Foot( foot ),
		m_Side( side ),
		m_Index( index ),
		m_Rank( rank )
		{
		}

		Leg::~Leg()
		{
		}

		Vec3 Leg::GetContactForce() const
		{
			SCONE_PROFILE_SCOPE;
			return m_Foot.GetBody().GetContactForce();
		}

		Real Leg::MeasureLength() const
		{
			// HACK: this uses body positions because joint positions are too hard for OpenSim
			// TODO: do it right
			// OpenSim: how can we get the actual position of a joint
			return ( m_Upper.GetParent().GetBody().GetPos() - m_Foot.GetBody().GetPos() ).GetLength();
		}
	}
}
