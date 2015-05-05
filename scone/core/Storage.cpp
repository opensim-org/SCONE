#include "stdafx.h"

#include "Storage.h"
#include "Exception.h"
#include <algorithm>

namespace scone
{

	Storage::Storage()
	{
	}

	Storage::~Storage()
	{
	}

	void Storage::SetValue( TimeInSeconds time, const String& label, const ValueType& value )
	{
		// see if we need to add a new frame
		if ( m_Data.empty() || m_Data.back().first != time )
			AddFrame( time );

		// see if this label is new
		SetValue( label, value );
	}

	void Storage::SetValue( const String& label, const ValueType& value )
	{
		SCONE_ASSERT( !m_Data.empty() );

		SetValue( GetOrAddChannel( label ), value );
	}

	void Storage::SetValue( Index idx, const ValueType& value )
	{
		SCONE_ASSERT( !m_Data.empty() );

		m_Data.back().second[ idx ] = value;
	}

	Storage::Frame& Storage::AddFrame( TimeInSeconds time )
	{
		SCONE_CONDITIONAL_THROW( !m_Data.empty() && time <= m_Data.back().first, "Frame must have higher timestamp" );
		m_Data.push_back( Frame( time, std::vector< ValueType >( m_Labels.size(), ValueType( 0 ) ) ) );
		m_BetweenFramesCache.clear(); // cached iterators have become invalid
		return m_Data.back();
	}

	size_t Storage::GetChannelCount() const
	{
		return m_Labels.size();
	}

	Index Storage::AddChannel( const String& label )
	{
		SCONE_ASSERT( m_Labels.find( label ) == m_Labels.end() );

		// create new channel and resize data
		auto label_iter = m_Labels.insert( std::make_pair( label, m_Labels.size() ) ).first;
		for ( auto it = m_Data.begin(); it != m_Data.end(); ++it )
			it->second.resize( m_Labels.size(), ValueType( 0 ) );
		return label_iter->second;
	}

	Index Storage::GetOrAddChannel( const String& label )
	{
		auto label_iter = m_Labels.find( label );
		if ( label_iter == m_Labels.end() )
			return AddChannel( label );
		else return label_iter->second;
	}

	scone::Index Storage::GetChannel( const String& label ) const
	{
		auto label_iter = m_Labels.find( label );
		SCONE_CONDITIONAL_THROW( label_iter == m_Labels.end(), "Could not find channel: " + label );
		return label_iter->second;
	}

	Storage::Frame& Storage::GetFrame( Index frame_idx )
	{
		SCONE_ASSERT( frame_idx < m_Data.size() );
		return m_Data[ frame_idx ];
	}

	Storage::ValueType Storage::GetValue( Index channel_idx ) const
	{
		SCONE_ASSERT( !m_Data.empty() );
		return m_Data.back().second[ channel_idx ];
	}

	Storage::ValueType Storage::GetValue( Index frame_idx, Index channel_idx ) const
	{
		SCONE_ASSERT( frame_idx < m_Data.size() );
		return m_Data[ frame_idx ].second[ channel_idx ];
	}

	Storage::ValueType Storage::GetInterpolatedValue( TimeInSeconds time, Index idx ) const
	{
		SCONE_ASSERT( !m_Data.empty() );

		return GetBetweenFrames( time ).value( idx );

		// find the first Frame with a higher timestamp than time, using cache
		//std::vector< Frame >::const_iterator it1;
		//auto cacheIt = m_BetweenFramesCache.find( time );
		//if ( cacheIt == m_BetweenFramesCache.end() )
		//{
		//	// compute the actual iterator position
		//	it1 = std::upper_bound( m_Data.cbegin(), m_Data.cend(), Frame( time, std::vector< ValueType >() ) );
		//	m_BetweenFramesCache[ time ] = it1; // cache iterator
		//}
		//else it1 = cacheIt->second; // we've found a cached version, no need to compute!

		//// check if we have enough data
		//if ( it1 == m_Data.cend() )
		//	return m_Data.back().second[ idx ]; // timestamp too high, return most recent value
		//if ( it1 == m_Data.cbegin() )
		//	return m_Data.front().second[ idx ]; // timestamp too low, return oldest value

		//auto it0 = it1;
		//--it0;

		//// return weighted average
		//double w = ( time - it0->first ) / ( it1->first - it0->first );
		//return w * it1->second[ idx ] + ( 1.0 - w ) * it0->second[ idx ];
	}

	Storage::BetweenFrames Storage::GetBetweenFrames( TimeInSeconds time ) const
	{
		// check the cache
		auto cacheIt = m_BetweenFramesCache.find( time );
		if ( cacheIt != m_BetweenFramesCache.end() )
			return cacheIt->second;

		// compute new one
		BetweenFrames bf;
		bf.it1 = std::upper_bound( m_Data.cbegin(), m_Data.cend(), Frame( time, std::vector< ValueType >() ) );
		if ( bf.it1 == m_Data.cend() )
		{
			// timestamp too high, point to most recent frame
			bf.it0 = bf.it1 = m_Data.begin() + m_Data.size() - 1;
			bf.w1 = 1.0;
		}
		else if ( bf.it1 == m_Data.cbegin() )
		{
			// timestamp too low, point to oldest frame
			bf.it0 = bf.it1;
			bf.w1 = 1.0;
		}
		else
		{
			// we have an actual interpolation
			bf.it0 = bf.it1 - 1;
			bf.w1 = ( time - bf.it0->first ) / ( bf.it1->first - bf.it0->first );
		}

		// store in cache
		m_BetweenFramesCache[ time ] = bf;

		return bf;
	}
}
