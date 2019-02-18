/*
** Quat.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "xo/geometry/quat.h"

namespace scone
{
	// import Quat from xo
	using Quat = xo::quat_< Real >;
	using Quatf = xo::quat_< float >;
	using Quatd = xo::quat_< double >;
}
