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
	if ( storage ) return storage->GetChannelCount();
	else return 0;
}

QString SconeStorageDataModel::label( int idx ) const
{
	SCONE_ASSERT( storage );
	return QString( storage->GetLabels()[ idx ].c_str() );
}

double SconeStorageDataModel::value( int idx, double time ) const
{
	SCONE_ASSERT( storage );
	double reltime = time / storage->Back().GetTime();
	int frame_idx = xo::clamped< int >( reltime * ( storage->GetFrameCount() - 1 ), 0, storage->GetFrameCount() - 1 );
	return storage->GetFrame( frame_idx )[ idx ];
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
