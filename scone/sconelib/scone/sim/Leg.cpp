#include "Leg.h"
#include "Link.h"
#include "Body.h"
#include "Joint.h"
#include "scone/core/Profiler.h"
#include "Model.h"

namespace scone
{
	namespace sim
	{
		Leg::Leg( Link& proximal, Link& foot, size_t index, Side side, size_t rank ) :
		m_Upper( proximal ),
		m_Foot( foot ),
		m_Side( side ),
		m_Index( index ),
		m_Rank( rank ),
		m_Name( stringf( "Leg%d", index ) + ( ( side == LeftSide ) ? "_l" : "_r" ) )
		{
			// measure length during construction, as it could be pose-dependent
			m_LegLength = MeasureLength();
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
			// HACK: this uses body positions because we don't have access to joint positions in OpenSim
			// OpenSim: how can we get the actual position of a joint
			// add all distances from foot to upper, using body origins
			double d = 0.0;
			for ( const Link* body = &m_Foot; body != &m_Upper; body = &body->GetParent() )
				d += length( body->GetBody().GetOrigin() - body->GetParent().GetBody().GetOrigin() );

			return d;
		}

		scone::Real Leg::GetLoad() const
		{
			return GetContactForce().y / GetModel().GetBW();
		}

		Model& Leg::GetModel() const
		{
			return m_Upper.GetBody().GetModel();
		}
	}
}
