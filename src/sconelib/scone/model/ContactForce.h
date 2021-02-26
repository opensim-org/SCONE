/*
** ContactForce.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/core/Vec3.h"
#include "scone/core/HasName.h"
#include "scone/core/HasData.h"
#include "ForceValue.h"

namespace scone
{
	class SCONE_API ContactForce : public HasName, HasData
	{
	public:
		ContactForce();
		virtual ~ContactForce();

		virtual const Vec3& GetForce() const = 0;
		virtual const Vec3& GetMoment() const = 0;
		virtual const Vec3& GetPoint() const = 0;
		virtual std::tuple<const Vec3&, const Vec3&, const Vec3&> GetForceMomentPoint() const;
		virtual ForceValue GetForceValue() const;

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;

		const std::vector< ContactGeometry* >& GetContactGeometries() const { return m_Geometries; }

	protected:
		std::vector< ContactGeometry* > m_Geometries;
	};
}
