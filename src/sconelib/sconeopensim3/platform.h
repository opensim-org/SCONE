/*
** platform.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#if defined(_MSC_VER)
#	ifdef SCONE_MODEL_OPENSIM_3_EXPORTS
#		define SCONE_OPENSIM_3_API __declspec(dllexport)
#	else
#		define SCONE_OPENSIM_3_API __declspec(dllimport)
#	endif
#else
#	define SCONE_OPENSIM_3_API
#endif
