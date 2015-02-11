#include "stdafx.h"

#include "Leg.h"
#include "Link.h"
#include "Body.h"

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
			return m_Foot.GetBody().GetContactForce();
		}
	}
}
