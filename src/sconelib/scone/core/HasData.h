/*
** HasData.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
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
		MuscleActivation,
		MuscleExcitation,
		MuscleFiberProperties,
		JointReactionForce,
		JointMoment,
		BodyOriginPosition,
		BodyComPosition,
		BodyOrientation,
		GroundReactionForce,
		CenterOfMass,
		SensorData,
		ControllerData
	};

	using StoreDataFlags = xo::flag_set< StoreDataTypes >;

	class SCONE_API HasData
	{
	public:
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const = 0;
		inline virtual ~HasData() {}
	};
}
