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

QString SconeStorageDataModel::label( int idx ) const
{
	return storage ? QString( storage->GetLabels()[ idx ].c_str() ) : QString();
}

double SconeStorageDataModel::value( int idx, double time ) const
{
	return storage ? storage->GetFrame( timeIndex( time ) )[ idx ] : 0;
}

std::vector< std::pair< float, float > > SconeStorageDataModel::getSeries( int idx, double min_interval ) const
{
	std::vector< std::pair< float, float > > series;
	if ( storage )
	{
		series.reserve( storage->GetFrameCount() ); // this may be a little much, but ensures no reallocation
		double last_time = timeStart() - 2 * min_interval;
		for ( size_t i = 0; i < storage->GetFrameCount(); ++i )
		{
			auto& f = storage->GetFrame( i );
			if ( f.GetTime() - last_time >= min_interval )
			{
				series.emplace_back( static_cast<float>( f.GetTime() ), static_cast<float>( f[ idx ] ) );
				last_time = f.GetTime();
			}
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
	if ( storage && !storage->IsEmpty() )
	{
		double reltime = time / storage->Back().GetTime();
		return xo::index_t( xo::clamped< int >( reltime * ( storage->GetFrameCount() - 1 ) + 0.5, 0, storage->GetFrameCount() - 1 ) );
	}
	else return xo::no_index;
}

double SconeStorageDataModel::timeValue( xo::index_t idx ) const
{
	return storage && idx != xo::no_index ? storage->GetFrame( idx ).GetTime() : 0.0;
}
