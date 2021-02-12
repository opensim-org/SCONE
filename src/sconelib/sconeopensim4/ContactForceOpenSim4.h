/*
** ContactForceOpenSim4.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/ContactForce.h"
#include "ModelOpenSim4.h"

namespace OpenSim
{
	class HuntCrossleyForce;
}

namespace scone
{
	class SCONE_OPENSIM_4_API ContactForceOpenSim4 : public ContactForce
	{
	public:
		ContactForceOpenSim4( class ModelOpenSim4& model, const OpenSim::HuntCrossleyForce& force );
		virtual ~ContactForceOpenSim4();

		virtual const String& GetName() const override;

		virtual const Vec3& GetForce() const override;
		virtual const Vec3& GetMoment() const override;
		virtual const Vec3& GetPoint() const override;
		virtual std::tuple<const Vec3&, const Vec3&, const Vec3&> GetForceMomentPoint() const override;
		ForceValue GetForceValue() const override;

	private:
		const OpenSim::HuntCrossleyForce& m_osForce;
		ModelOpenSim4& m_Model;

		mutable Vec3 m_Force;
		mutable Vec3 m_Moment;
		mutable Vec3 m_Point;

		void UpdateForceValues() const;
		mutable int m_LastNumDynamicsRealizations;
		mutable std::vector< Real > m_Values;
		std::vector< String > m_Labels;
		Vec3 m_PlaneNormal;
		Vec3 m_PlaneLocation;
	};
}
