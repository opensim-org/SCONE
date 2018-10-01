/*
** studio_config.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

namespace scone
{
#ifdef _MSC_VER
#	define SCONE_SCONECMD_EXECUTABLE "sconecmd.exe"
#	define SCONE_FFMPEG_EXECUTABLE "ffmpeg.exe"
#else
#	define SCONE_SCONECMD_EXECUTABLE "sconecmd"
#	define SCONE_FFMPEG_EXECUTABLE "ffmpeg"
#endif
}
