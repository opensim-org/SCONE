/*
** HasData.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Storage.h"
#include "xo/container/flag_set.h"

namespace scone
{
	enum class StoreDataTypes {
		Unknown,
		State,
		BodyPosition,
		JointReactionForce,
		ActuatorInput,
		MuscleProperties,
		MuscleDofMomentPower,
		GroundReactionForce,
		ContactForce,
		SystemPower,
		SensorData,
		ControllerData,
		MeasureData,
		SimulationStatistics,
		DebugData
	};

	using StoreDataFlags = xo::flag_set< StoreDataTypes >;

	/// Objects derived from this class can store data for analysis
	class SCONE_API HasData
	{
	public:
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const = 0;
		inline virtual ~HasData() {}
	};
}
