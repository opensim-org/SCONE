/*
** Vec3.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "types.h"
#include <xo/geometry/vec3.h>

namespace scone
{
	// import Vec3 from xo
	typedef xo::vec3_< Real > Vec3;
	typedef xo::vec3_< float > Vec3f;
	typedef xo::vec3_< double > Vec3d;
}
