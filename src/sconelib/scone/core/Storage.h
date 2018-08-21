#pragma once

#include "core.h"
#include "types.h"
#include "Exception.h"

#include <memory>
#include <map>
#include <unordered_map>
#include <vector>
#include <utility>
#include <algorithm>

namespace scone
{
	template< typename ValueT = Real, typename TimeT = TimeInSeconds >
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
			m_Values( store.GetChannelCount(), default_value ) { }

			TimeT GetTime() const { return m_Time; }

			ValueT& operator[]( index_t idx ) { return m_Values[ idx ]; }

			const ValueT& operator[]( index_t idx ) const { return m_Values[ idx ]; }

			ValueT& operator[]( const String& label ) {
				index_t idx = m_Store.GetChannelIndex( label );
				if ( idx == NoIndex )
					idx = m_Store.AddChannel( label );
				return m_Values[ idx ];
			}

			const ValueT& operator[]( const String& label ) const {
				index_t idx = m_Store.GetChannelIndex( label );
				SCONE_ASSERT( idx != NoIndex );
				return m_Values[ idx ];
			}

			const std::vector< ValueT > GetValues() { return m_Values; }

		private:
			Storage< ValueT, TimeT >& m_Store;
			TimeT m_Time;
			std::vector< ValueT > m_Values;
		};
		typedef std::unique_ptr< Frame > FrameUP;

		Storage() { };
		Storage( const Storage& other ) {
			*this = other;
		};
		Storage( Storage&& other ) {
			*this = std::move( other );
		};
		Storage& operator=( const Storage& other ) {
			m_Labels = other.m_Labels;
			m_LabelIndexMap = other.m_LabelIndexMap;
			m_Data.clear();
			m_Data.reserve( other.m_Data.size() );
			for ( auto it = other.m_Data.begin(); it != other.m_Data.end(); ++it )
				m_Data.push_back( FrameUP( new Frame( **it ) ) );
			m_InterpolationCache.clear();
			return *this;
		};
		Storage& operator=( Storage&& other ) {
			m_Labels = std::move( other.m_Labels );
			m_LabelIndexMap = std::move( other.m_LabelIndexMap );
			m_Data = std::move( other.m_Data );
			m_InterpolationCache.clear();
			return *this;
		};
		~Storage() { };

		void Clear() { m_Labels.clear(); m_LabelIndexMap.clear(); m_Data.clear(); m_InterpolationCache.clear(); }

		Storage CopySlice( size_t start, size_t size, size_t stride ) const {
			SCONE_ASSERT( stride > 0 );
			Storage r;
			r.m_Labels = m_Labels;
			r.m_LabelIndexMap = m_LabelIndexMap;
			if ( size == 0 || size > GetFrameCount() / stride )
				size = ( GetFrameCount() + stride - 1 ) / stride;
			r.m_Data.reserve( size );
			for ( size_t i = start; r.m_Data.size() < size && i < m_Data.size(); i += stride )
				r.m_Data.push_back( FrameUP( new Frame( *m_Data[ i ] ) ) );
			return r;
		}

		Frame& AddFrame( TimeT time, ValueT default_value = ValueT( 0 ) ) {
			SCONE_THROW_IF( !m_Data.empty() && time <= m_Data.back()->GetTime(), "Frame must have higher timestamp" );
			m_Data.push_back( FrameUP( new Frame( *this, time, default_value ) ) );
			m_InterpolationCache.clear(); // cached iterators have become invalid
			return *m_Data.back();
		}
		
		bool IsEmpty() const { return m_Data.empty(); }

		Frame& Back() { SCONE_ASSERT( !m_Data.empty() ); return *m_Data.back(); }
		const Frame& Back() const { SCONE_ASSERT( !m_Data.empty() ); return *m_Data.back(); }

		Frame& GetFrame( index_t frame_idx ) { SCONE_ASSERT( frame_idx < m_Data.size() ); return *m_Data[ frame_idx ]; }
		const Frame& GetFrame( index_t frame_idx ) const { SCONE_ASSERT( frame_idx < m_Data.size() ); return *m_Data[ frame_idx ]; }

		std::vector< ValueT > GetChannelData( index_t idx ) const {
			std::vector< ValueT > result( GetFrameCount() );
			for ( index_t f = 0; f < GetFrameCount(); ++f )
				result[ f ] = GetFrame( f )[ idx ];
			return result;
		}

		size_t GetFrameCount() const { return m_Data.size(); }

		index_t AddChannel( const String& label, ValueT default_value = ValueT( 0 ) ) {
			SCONE_ASSERT( GetChannelIndex( label ) == NoIndex );
			m_Labels.push_back( label );
			m_LabelIndexMap[ label ] = m_Labels.size() - 1;
			for ( auto it = m_Data.begin(); it != m_Data.end(); ++it )
				(*it)->m_Values.resize( m_Labels.size(), default_value ); // resize existing data
			return m_Labels.size() - 1;
		}

		index_t GetChannelIndex( const String& label ) const {
			auto it = m_LabelIndexMap.find( label );
			if ( it == m_LabelIndexMap.end() )
				return NoIndex;
			else return it->second;
		}

		size_t GetChannelCount() const { return m_Labels.size(); }
		const std::vector< String >& GetLabels() const { return m_Labels; }
		const std::vector< FrameUP >& GetData() const { return m_Data; }

		ValueT GetInterpolatedValue( TimeT time, index_t idx ) const {
			SCONE_ASSERT( !m_Data.empty() );
			return GetInterpolatedFrame( time ).value( idx );
		}

	private:
		std::vector< String > m_Labels;
		std::vector< std::unique_ptr< Frame > > m_Data;
		std::unordered_map< String, index_t > m_LabelIndexMap;

		// interpolation related stuff
		struct InterpolatedFrame {
			double upper_weight;
			typename std::vector< FrameUP >::const_iterator upper_frame, lower_frame;
			ValueT value( index_t channel_idx ) const { return upper_weight * (**upper_frame)[ channel_idx ] + ( 1.0 - upper_weight ) * (**lower_frame)[ channel_idx ]; }
		};

	public:
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

	private:
		mutable std::map< TimeT, InterpolatedFrame > m_InterpolationCache;
	};
}
