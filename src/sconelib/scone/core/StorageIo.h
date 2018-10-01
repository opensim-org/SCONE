/*
** StorageIo.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "Storage.h"
#include <fstream>
#include "xo/serialization/char_stream.h"

namespace scone
{
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str );
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, const xo::path& file );

	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, const xo::path& file, const String& name );
	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str, const String& name );

	void SCONE_API ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, const xo::path& file );
	void SCONE_API ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, xo::char_stream& str );

	void SCONE_API ReadStorageSto( Storage< Real, TimeInSeconds >& storage, const xo::path& file );
	void SCONE_API ReadStorageSto( Storage< Real, TimeInSeconds >& storage, xo::char_stream& str );
}
