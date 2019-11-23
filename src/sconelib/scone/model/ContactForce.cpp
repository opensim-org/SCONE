/*
** ContactForce.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ContactForce.h"

namespace scone
{
	ContactForce::ContactForce()
	{}

	ContactForce::~ContactForce()
	{}

	std::tuple<const Vec3&, const Vec3&, const Vec3&> ContactForce::GetForceMomentPoint() const
	{
		return { GetForce(), GetMoment(), GetPoint() };
	}

	ForceValue ContactForce::GetForceValue() const
	{
		return ForceValue{ GetForce(), GetPoint() };
	}

	void ContactForce::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
	        const auto& [ force, moment, point ]= GetForceMomentPoint();
		frame[ GetName() + ".force_x" ] = force.x;
		frame[ GetName() + ".force_y" ] = force.y;
		frame[ GetName() + ".force_z" ] = force.z;
		frame[ GetName() + ".moment_x" ] = moment.x;
		frame[ GetName() + ".moment_y" ] = moment.y;
		frame[ GetName() + ".moment_z" ] = moment.z;
	}
}
