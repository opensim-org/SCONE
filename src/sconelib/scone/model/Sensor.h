/*
** Sensor.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/core/platform.h"

namespace scone
{
	struct Sensor
	{
		Sensor() {}
		virtual ~Sensor() {}
		virtual String GetName() const = 0;
		virtual Real GetValue() const = 0;
	};
}
