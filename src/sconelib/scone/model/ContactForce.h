/*
** ContactForce.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/core/Vec3.h"
#include "scone/core/HasName.h"
#include "scone/core/HasData.h"

namespace scone
{
	class SCONE_API ContactForce : public HasName, HasData
	{
	public:
		ContactForce();
		virtual ~ContactForce();

		virtual Vec3 GetForce() const = 0;
		virtual Vec3 GetMoment() const = 0;
		virtual Vec3 GetPoint() const = 0;

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
	};
}
