/*
** StorageIo.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "Storage.h"
#include "xo/serialization/char_stream.h"
#include <iosfwd>
#include <cstdio>

namespace scone
{
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, std::ostream& str, const String& time_label, TimeInSeconds min_interval = 0.0 );
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, std::FILE* f, const String& time_label, TimeInSeconds min_interval = 0.0 );
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, const xo::path& file, const String& time_label, TimeInSeconds min_interval = 0.0 );

	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, const xo::path& file, const String& name, TimeInSeconds min_interval = 0.0 );
	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, std::FILE*, const String& name, TimeInSeconds min_interval = 0.0 );
	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, std::ostream& str, const String& name, TimeInSeconds min_interval = 0.0 );

	void SCONE_API ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, const xo::path& file );
	void SCONE_API ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, xo::char_stream& str );

	void SCONE_API ReadStorageSto( Storage< Real, TimeInSeconds >& storage, const xo::path& file );
	void SCONE_API ReadStorageSto( Storage< Real, TimeInSeconds >& storage, xo::char_stream& str );
}
