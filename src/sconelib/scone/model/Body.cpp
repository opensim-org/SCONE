#include "Body.h"

namespace scone
{
	Body::Body()
	{
	}

	Body::~Body()
	{
	}

	void Body::ClearExternalForceAndMoment()
	{
		SetExternalForce( Vec3::zero() );
		SetExternalMoment( Vec3::zero() );
	}
}
