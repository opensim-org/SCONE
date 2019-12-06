/*
** Body.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Body.h"
#include "../core/HasData.h"

namespace scone
{
	Body::Body() :
		m_Joint( nullptr )
	{}

	Body::~Body()
	{}

	void Body::ClearExternalForceAndMoment()
	{
		SetExternalForce( Vec3::zero() );
		SetExternalMoment( Vec3::zero() );
	}

	void Body::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		auto& name = GetName();
		if ( flags( StoreDataTypes::BodyComPosition ) )
		{
			auto pos = GetComPos();
			frame[ name + ".com_pos_x" ] = pos.x;
			frame[ name + ".com_pos_y" ] = pos.y;
			frame[ name + ".com_pos_z" ] = pos.z;
			auto lin_vel = GetComVel();
			frame[ name + ".lin_vel_x" ] = lin_vel.x;
			frame[ name + ".lin_vel_y" ] = lin_vel.y;
			frame[ name + ".lin_vel_z" ] = lin_vel.z;
		}
		if ( flags( StoreDataTypes::BodyOrientation ) )
		{
			auto ori = rotation_vector_from_quat( normalized( GetOrientation() ) );
			frame[ name + ".ori_x" ] = ori.x;
			frame[ name + ".ori_y" ] = ori.y;
			frame[ name + ".ori_z" ] = ori.z;
			auto ang_vel = GetAngVel();
			frame[ name + ".ang_vel_x" ] = ang_vel.x;
			frame[ name + ".ang_vel_y" ] = ang_vel.y;
			frame[ name + ".ang_vel_z" ] = ang_vel.z;
		}
	}
}
