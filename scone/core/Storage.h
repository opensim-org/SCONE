#pragma once

#include "core.h"

#include <map>
#include <vector>
#include <utility>

namespace scone
{
	class CORE_API Storage
	{
	public:
		typedef double ValueType;
		typedef std::pair< TimeInSeconds, std::vector< ValueType > > Frame;

		Storage();
		virtual ~Storage();

		Index AddChannel( const String& label );
		Index GetOrAddChannel( const String& label );
		Index GetChannel( const String& label ) const;
		size_t GetChannelCount() const;

		Frame& AddFrame( TimeInSeconds time );
		Frame& GetFrame( Index frame_idx );

		void SetValue( TimeInSeconds time, const String& label, const ValueType& value );
		void SetValue( const String& label, const ValueType& value );
		void SetValue( Index idx, const ValueType& value );

		ValueType GetValue( Index channel_idx ) const;
		ValueType GetValue( Index frame_idx, Index channel_idx ) const;
		ValueType GetInterpolatedValue( TimeInSeconds time, Index idx ) const;

	private:
		// TODO: use std::vector< String > for labels
		std::map< String, size_t > m_Labels;
		std::vector< Frame > m_Data;

		// interpolation related stuff
		struct BetweenFrames {
			double w1;
			std::vector< Frame >::const_iterator it1, it0;
			ValueType value( Index channel_idx ) const { return w1 * it1->second[ channel_idx ] + ( 1.0 - w1 ) * it0->second[ channel_idx ]; }
		};
		BetweenFrames GetBetweenFrames( TimeInSeconds ) const;
		mutable std::map< TimeInSeconds, BetweenFrames > m_BetweenFramesCache;
	};
}
