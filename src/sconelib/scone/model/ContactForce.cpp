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

	void ContactForce::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		auto force = GetForce();
		auto moment = GetMoment();
		frame[ GetName() + ".contact_force_x" ] = force.x;
		frame[ GetName() + ".contact_force_y" ] = force.y;
		frame[ GetName() + ".contact_force_z" ] = force.z;
		frame[ GetName() + ".contact_moment_x" ] = moment.x;
		frame[ GetName() + ".contact_moment_y" ] = moment.y;
		frame[ GetName() + ".contact_moment_z" ] = moment.z;
	}
}
