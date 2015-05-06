#pragma once

#include "core.h"

#include <map>
#include <vector>
#include <utility>

namespace scone
{
	template< typename ValueT, typename TimeT = TimeInSeconds >
	class Storage
	{
	public:
		typedef std::pair< TimeT, std::vector< ValueT > > Frame;

		Storage() { };
		~Storage() { };

		Index AddChannel( const String& label, ValueT default_value = ValueT( 0 ) ) {
			SCONE_ASSERT( GetChannelIndex( label ) == NoIndex );
			m_Labels.push_back( label );
			for ( auto it = m_Data.begin(); it != m_Data.end(); ++it )
				it->second.resize( m_Labels.size(), default_value ); // resize existing data
			return m_Labels.size() - 1;
		}

		Index GetChannelIndex( const String& label ) const {
			auto it = std::find( m_Labels.begin(), m_Labels.end(), label );
			if ( it == m_Labels.end() )
				return NoIndex;
			else return it - m_Labels.begin();
		}

		size_t GetChannelCount() const {
			return m_Labels.size();
		}

		void AddFrame( TimeT time, ValueT default_value = ValueT( 0 ) ) {
			SCONE_CONDITIONAL_THROW( !m_Data.empty() && time <= m_Data.back().first, "Frame must have higher timestamp" );
			m_Data.push_back( Frame( time, std::vector< ValueT >( m_Labels.size(), default_value ) ) );
			m_InterpolationCache.clear(); // cached iterators have become invalid
		}

		ValueT& operator[]( Index idx ) {
			SCONE_ASSERT( !m_Data.empty() );
			return m_Data.back().second[ idx ];
		}

		const ValueT& operator[]( Index idx ) const {
			SCONE_ASSERT( !m_Data.empty() );
			return m_Data.back().second[ idx ];
		}

		ValueT& operator[]( const String& label ) {
			SCONE_ASSERT( !m_Data.empty() );
			Index idx = GetChannelIndex( label );
			if ( idx == NoIndex )
				idx = AddChannel( label );
			return m_Data.back().second[ idx ];
		}

		const ValueT& operator[]( const String& label ) const {
			SCONE_ASSERT( !m_Data.empty() );
			Index idx = GetChannelIndex( label );
			SCONE_ASSERT( idx != NoIndex );
			return m_Data.back().second[ idx ];
		}

		const ValueT& GetValue( Index frame_idx, Index channel_idx ) const {
			SCONE_ASSERT( frame_idx < m_Data.size() );
			return m_Data[ frame_idx ].second[ channel_idx ];
		}

		const TimeT& GetTime( Index frame_idx ) const {
			SCONE_ASSERT( frame_idx < m_Data.size() );
			return m_Data[ frame_idx ].first;
		}

		ValueT GetInterpolatedValue( TimeT time, Index idx ) const {
			SCONE_ASSERT( !m_Data.empty() );
			return GetInterpolatedFrame( time ).value( idx );
		}

	private:
		std::vector< String > m_Labels;
		std::vector< Frame > m_Data;

		// interpolation related stuff
		struct InterpolatedFrame {
			double upper_weight;
			typename std::vector< Frame >::const_iterator upper_frame, lower_frame;
			ValueT value( Index channel_idx ) const { return upper_weight * upper_frame->second[ channel_idx ] + ( 1.0 - upper_weight ) * lower_frame->second[ channel_idx ]; }
		};

		InterpolatedFrame GetInterpolatedFrame( TimeT time ) const {
			// check the cache
			auto cacheIt = m_InterpolationCache.find( time );
			if ( cacheIt != m_InterpolationCache.end() )
				return cacheIt->second;

			// compute new one
			InterpolatedFrame bf;
			bf.upper_frame = std::upper_bound( m_Data.cbegin(), m_Data.cend(), Frame( time, std::vector< ValueT >() ) );
			if ( bf.upper_frame == m_Data.cend() )
			{
				// timestamp too high, point to most recent frame
				bf.lower_frame = bf.upper_frame = m_Data.begin() + m_Data.size() - 1;
				bf.upper_weight = 1.0;
			}
			else if ( bf.upper_frame == m_Data.cbegin() )
			{
				// timestamp too low, point to oldest frame
				bf.lower_frame = bf.upper_frame;
				bf.upper_weight = 1.0;
			}
			else
			{
				// we have an actual interpolation
				bf.lower_frame = bf.upper_frame - 1;
				bf.upper_weight = ( time - bf.lower_frame->first ) / ( bf.upper_frame->first - bf.lower_frame->first );
			}

			// store in cache
			m_InterpolationCache[ time ] = bf;

			return bf;

		}
		mutable std::map< TimeT, InterpolatedFrame > m_InterpolationCache;
	};
}
