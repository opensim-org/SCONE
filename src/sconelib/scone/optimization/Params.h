/*
** Params.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "spot/search_point.h"
#include "spot/objective_info.h"
#include "spot/par_tools.h"
#include "spot/par_info.h"

namespace scone
{
	using Params = spot::par_io;
	using SearchPoint = spot::search_point;
	using ObjectiveInfo = spot::objective_info;
	using ParInfo = spot::par_info;
	using ScopedParamSetPrefixer = spot::scoped_prefix;
}
