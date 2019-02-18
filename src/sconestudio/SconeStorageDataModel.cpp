/*
** SconeStorageDataModel.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SconeStorageDataModel.h"
#include "xo/numerical/math.h"

SconeStorageDataModel::SconeStorageDataModel( const scone::Storage<>* s ) : storage( s )
{}

void SconeStorageDataModel::setStorage( const scone::Storage<>* s )
{
	storage = s;
}

size_t SconeStorageDataModel::seriesCount() const
{
	return storage ? storage->GetChannelCount() : 0;
}

size_t SconeStorageDataModel::samplesCount() const
{
	return storage ? storage->GetFrameCount() : 0;
}

QString SconeStorageDataModel::label( int idx ) const
{
	SCONE_ASSERT( storage );
	return QString( storage->GetLabels()[ idx ].c_str() );
}

double SconeStorageDataModel::value( int idx, double time ) const
{
	return storage->GetFrame( timeIndex( time ) )[ idx ];
}

std::vector< std::pair< float, float > > SconeStorageDataModel::getSeries( int idx, double min_interval ) const
{
	SCONE_ASSERT( storage );
	std::vector< std::pair< float, float > > series;
	series.reserve( storage->GetFrameCount() ); // this may be a little much, but ensures no reallocation
	double last_time = timeStart() - 2 * min_interval;
	for ( size_t i = 0; i < storage->GetFrameCount(); ++i )
	{
		auto& f = storage->GetFrame( i );
		if ( f.GetTime() - last_time >= min_interval )
		{
			series.emplace_back( static_cast< float >( f.GetTime() ), static_cast< float >( f[ idx ] ) );
			last_time = f.GetTime();
		}
	}
	return series;
}

double SconeStorageDataModel::timeFinish() const
{
	return storage && !storage->IsEmpty() ? storage->Back().GetTime() : 0.0;
}

double SconeStorageDataModel::timeStart() const
{
	return 0.0;
}

xo::index_t SconeStorageDataModel::timeIndex( double time ) const
{
	SCONE_ASSERT( storage );
	double reltime = time / storage->Back().GetTime();
	return xo::index_t( xo::clamped< int >( reltime * ( storage->GetFrameCount() - 1 ) + 0.5, 0, storage->GetFrameCount() - 1 ) );
}

double SconeStorageDataModel::timeValue( xo::index_t idx ) const
{
	return storage->GetFrame( idx ).GetTime();
}
