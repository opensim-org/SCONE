/*
** StorageIo.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "StorageIo.h"

#include "xo/string/string_tools.h"
#include "xo/filesystem/path.h"

namespace scone
{
	
	void WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str )
	{
		// write data
		str << "time";
		for ( const String& label : storage.GetLabels() )
			str << "\t" << label;
		str << std::endl;

		for ( auto& frame : storage.GetData() )
		{
			str << frame->GetTime();
			for ( size_t idx = 0; idx < storage.GetChannelCount(); ++idx )
				str << "\t" << ( *frame )[idx];
			str << std::endl;
		}
	}

	void WriteStorageTxt( const Storage< Real, TimeInSeconds >& storage, const xo::path& file )
	{
		std::ofstream ofs( file.str() );
		SCONE_ASSERT_MSG( ofs.good(), "Error opening file " + file.str() );
		WriteStorageTxt( storage, ofs );
	}

	void WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, std::ofstream& str, const String& name )
	{
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

	void WriteStorageSto( const Storage< Real, TimeInSeconds >& storage, const xo::path& file, const String& name )
	{
		std::ofstream str( file.str() );
		SCONE_ASSERT_MSG( str.good(), "Error opening file " + file.str() );
		WriteStorageSto( storage, str, name );
	}

	void ReadStorageSto( Storage< Real, TimeInSeconds >& storage, const xo::path& file )
	{
		auto str = xo::char_stream( file );
		SCONE_ASSERT_MSG( str.good(), "Error opening file " + file.str() );
		ReadStorageSto( storage, str );
	}

	void ReadStorageSto( Storage< Real, TimeInSeconds >& storage, xo::char_stream& str )
	{
		// skip the header since we don't need it
		String s;
		while ( s != "endheader" && str.good() )
			str >> s;

		if ( str.good() )
			ReadStorageTxt( storage, str ); // read as txt once we have found the header
	}

	void ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, const xo::path& file )
	{
		auto str = xo::char_stream( file );
		SCONE_ASSERT_MSG( str.good(), "Error opening file " + file.str() );
		ReadStorageTxt( storage, str );
	}

	void ReadStorageTxt( Storage< Real, TimeInSeconds >& storage, xo::char_stream& str )
	{
		storage.Clear();

		// read time label
		String dummy;
		str >> dummy;
		SCONE_ASSERT_MSG( dummy == "time", "First column should be labeled 'time'" );

		String header = str.get_line();
		SCONE_ASSERT_MSG( !str.fail(), "Error reading file labels" );
		auto labels = xo::split_str( header, "\t " );

		// add channels to storage
		for ( auto& s : labels )
			storage.AddChannel( s );

		// read the data
		while ( str.good() )
		{
			double time;
			str >> time;

			if ( !str.good() )
				return; // stop if timestamp could not be read;

			auto& frame = storage.AddFrame( time );
			for ( size_t i = 0; i < storage.GetChannelCount(); ++i )
			{
				double value = 0.0;
				str >> value;
				frame[ i ] = value;
			}
		}
	}
}
