#pragma once

#include "core.h"

#include <map>
#include <vector>
#include <utility>
#include <algorithm>

namespace scone
{
	template< typename ValueT, typename TimeT = TimeInSeconds >
	class Storage
	{
	public:
		class Frame
		{
		public:
			friend class Storage;

			Frame( Storage& store, TimeT t, ValueT default_value = ValueT( 0 ) ) :
			m_Store( store ),
			m_Time( t ),
			m_Values( store.GetChannelCount(), default_value ) {
			}

			TimeT GetTime() { return m_Time; }

			ValueT& operator[]( Index idx ) {
				return m_Values[ idx ];
			}

			const ValueT& operator[]( Index idx ) const {
				return m_Values[ idx ];
			}

			ValueT& operator[]( const String& label ) {
				Index idx = m_Store.GetChannelIndex( label );
				if ( idx == NoIndex )
					idx = m_Store.AddChannel( label );
				return m_Values[ idx ];
			}

			const ValueT& operator[]( const String& label ) const {
				Index idx = m_Store.GetChannelIndex( label );
				SCONE_ASSERT( idx != NoIndex );
				return m_Values[ idx ];
			}

		private:
			typename Storage< ValueT, TimeT >& m_Store;
			typename TimeT m_Time;
			typename std::vector< ValueT > m_Values;
		};
		typedef std::unique_ptr< Frame > FrameUP;

		Storage() { };
		Storage( const Storage& other ) {
			*this = other;
		};
		Storage& operator=( const Storage& other ) {
			for ( auto it = other.m_Data.begin(); it != other.m_Data.end(); ++it )
				m_Data.push_back( FrameUP( new Frame( **it ) ) );
			m_InterpolationCache.clear();
			return *this;
		};
		~Storage() { };

		Frame& AddFrame( TimeT time, ValueT default_value = ValueT( 0 ) ) {
			SCONE_THROW_IF( !m_Data.empty() && time <= m_Data.back()->GetTime(), "Frame must have higher timestamp" );
			m_Data.push_back( FrameUP( new Frame( *this, time, default_value ) ) );
			m_InterpolationCache.clear(); // cached iterators have become invalid
			return *m_Data.back();
		}
		
		bool IsEmpty() const { return m_Data.empty(); }

		Frame& Back() {
			SCONE_ASSERT( !m_Data.empty() );
			return *m_Data.back();
		}

		size_t GetFrameCount() const { return m_Data.size(); }

		Frame& GetFrame( Index frame_index ) {
			SCONE_ASSERT( frame_idx < m_Data.size() );
			return m_Data[ frame_idx ];
		}

		Index AddChannel( const String& label, ValueT default_value = ValueT( 0 ) ) {
			SCONE_ASSERT( GetChannelIndex( label ) == NoIndex );
			m_Labels.push_back( label );
			for ( auto it = m_Data.begin(); it != m_Data.end(); ++it )
				(*it)->m_Values.resize( m_Labels.size(), default_value ); // resize existing data
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

		ValueT GetInterpolatedValue( TimeT time, Index idx ) const {
			SCONE_ASSERT( !m_Data.empty() );
			return GetInterpolatedFrame( time ).value( idx );
		}

	private:
		std::vector< String > m_Labels;
		std::vector< std::unique_ptr< Frame > > m_Data;

		// interpolation related stuff
		struct InterpolatedFrame {
			double upper_weight;
			typename std::vector< FrameUP >::const_iterator upper_frame, lower_frame;
			ValueT value( Index channel_idx ) const { return upper_weight * (**upper_frame)[ channel_idx ] + ( 1.0 - upper_weight ) * (**lower_frame)[ channel_idx ]; }
		};

		InterpolatedFrame GetInterpolatedFrame( TimeT time ) const {
			// check the cache
			auto cacheIt = m_InterpolationCache.find( time );
			if ( cacheIt != m_InterpolationCache.end() )
				return cacheIt->second;

			// compute new one
			InterpolatedFrame bf;
			bf.upper_frame = std::upper_bound( m_Data.cbegin(), m_Data.cend(), time, []( TimeT lhs, const FrameUP &rhs) { return lhs < rhs->GetTime(); } );
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
				bf.upper_weight = ( time - (*bf.lower_frame)->GetTime() ) / ( (*bf.upper_frame)->GetTime() - (*bf.lower_frame)->GetTime() );
			}

			// store in cache
			m_InterpolationCache[ time ] = bf;

			return bf;

		}
		mutable std::map< TimeT, InterpolatedFrame > m_InterpolationCache;
	};
}
