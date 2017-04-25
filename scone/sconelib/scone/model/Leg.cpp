#include "Leg.h"
#include "Link.h"
#include "Body.h"
#include "Joint.h"
#include "scone/core/Profiler.h"
#include "Model.h"
#include "sim_tools.h"

namespace scone
{
	Leg::Leg( Link& proximal, Link& foot, size_t index, Side side, size_t rank ) :
		m_Upper( proximal ),
		m_Foot( foot ),
		m_Side( side ),
		m_Index( index ),
		m_Rank( rank ),
		m_Name( stringf( "leg%d", index ) + ( ( side == LeftSide ) ? "_l" : "_r" ) )
	{
		// measure length during construction, as it could be pose-dependent
		m_LegLength = MeasureLength();
	}

	Leg::~Leg()
	{
	}

	Vec3 Leg::GetContactForce() const
	{
		SCONE_PROFILE_FUNCTION;
		return m_Foot.GetBody().GetContactForce();
	}

	scone::Vec3 Leg::GetContactCop() const
	{
		return GetGroundCop( m_Foot.GetBody().GetContactForce(), m_Foot.GetBody().GetContactMoment() );
	}

	Vec3 Leg::GetRelFootPos() const
	{
		return GetFootLink().GetBody().GetComPos() - GetBaseLink().GetBody().GetComPos();
	}

	void Leg::GetContactForceMomentCop( Vec3& force, Vec3& moment, Vec3& cop ) const
	{
		SCONE_PROFILE_FUNCTION;
		force = m_Foot.GetBody().GetContactForce();
		moment = m_Foot.GetBody().GetContactMoment();
		cop = GetGroundCop( force, moment );
	}

	Real Leg::MeasureLength() const
	{
		// HACK: this uses body positions because we don't have access to joint positions in OpenSim
		// OpenSim: how can we get the actual position of a joint
		// add all distances from foot to upper, using body origins
		double d = 0.0;
		for ( const Link* body = &m_Foot; body != &m_Upper; body = &body->GetParent() )
			d += length( body->GetBody().GetOriginPos() - body->GetParent().GetBody().GetOriginPos() );

		return d;
	}

	scone::Real Leg::GetLoad() const
	{
		return GetContactForce().length() / GetModel().GetBW();
	}

	Model& Leg::GetModel() const
	{
		return m_Upper.GetBody().GetModel();
	}
}
