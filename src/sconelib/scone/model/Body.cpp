#include "Body.h"
#include "..\core\HasData.h"

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

	void Body::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		auto& name = GetName();
		if ( flags( StoreDataTypes::BodyOriginPosition ) )
		{
			auto pos = GetOriginPos();
			frame[ name + ".pos_x" ] = pos.x;
			frame[ name + ".pos_y" ] = pos.y;
			frame[ name + ".pos_z" ] = pos.z;
		}
		if ( flags( StoreDataTypes::BodyOrientation ) )
		{
			auto ori = rotation_vector_from_quat( GetOrientation() );
			frame[ name + ".ori_x" ] = ori.x;
			frame[ name + ".ori_y" ] = ori.y;
			frame[ name + ".ori_z" ] = ori.z;
		}
	}
}
