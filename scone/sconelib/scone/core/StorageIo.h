#pragma once

#include "core.h"
#include "Storage.h"
#include <fstream>
#include "flut/char_stream.hpp"

namespace scone
{
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str );
	void SCONE_API WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, const String& file );

	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, const String& file, const String& name );
	void SCONE_API WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str, const String& name );

	void SCONE_API ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, const String& file );
	void SCONE_API ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, flut::char_stream& str );

	void SCONE_API ReadStorageSto( Storage< Real, TimeInSeconds >& storage, const String& file );
	void SCONE_API ReadStorageSto( Storage< Real, TimeInSeconds >& storage, flut::char_stream& str );
}
