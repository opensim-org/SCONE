#pragma once

#include "core.h"
#include "Storage.h"
#include <fstream>

namespace scone
{
	void WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str )
	{
		// write data
		str << "time";
		for ( const String& label : storage.GetLables() )
			str << "\t" << label;
		str << std::endl;

		for ( auto& frame : storage.GetData() )
		{
			str << frame->GetTime();
			for ( size_t idx = 0; idx < storage.GetChannelCount(); ++idx )
				str << "\t" << ( *frame )[ idx ];
			str << std::endl;
		}
	}

	void WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, const String& file )
	{
		std::ofstream ofs( file );
        WriteStorageTxt( storage, ofs );
	}

	void WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, const String& file, const String& name )
	{
		std::ofstream str( file );

		// write header
		str << name << std::endl;
		str << "version=1" << std::endl;
		str << "nRows=" << storage.GetFrameCount() << std::endl;
		str << "nColumns=" << storage.GetChannelCount() + 1 << std::endl;
		str << "inDegrees=no" << std::endl;
		str << "endheader" << std::endl;

		// write data
		WriteStorageTxt( storage, str );
	}
}
