/*
** ContactForceOpenSim3.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/ContactForce.h"
#include "ModelOpenSim3.h"

namespace OpenSim
{
	class Force;
	class ModelOpenSim3;
}

namespace scone
{
	class SCONE_OPENSIM_3_API ContactForceOpenSim3 : public ContactForce
	{
	public:
		ContactForceOpenSim3( class ModelOpenSim3& model, OpenSim::Force& force );
		virtual ~ContactForceOpenSim3();

		virtual const String& GetName() const override;

		virtual Vec3 GetForce() const override;
		virtual Vec3 GetMoment() const override;
		virtual Vec3 GetPoint() const override;

	private:
		OpenSim::Force& m_osForce;

		mutable int m_LastNumDynamicsRealizations;
		mutable std::vector< Real > m_ContactForceValues;
		std::vector< String > m_ContactForceLabels;
	};
}
