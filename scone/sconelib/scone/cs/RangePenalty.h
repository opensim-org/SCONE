#pragma once

#include "scone/core/Range.h"

namespace scone
{
	/// Helper class to compute penalty if a value is outside a specific range
	template< typename T >
	class RangePenalty
	{
	public:
		RangePenalty( const PropNode& prop )
		{
			INIT_PROPERTY_NAMED( prop, range.min, "min", T( 0 ) );
			INIT_PROPERTY_NAMED( prop, range.max, "max", T( 0 ) );
			INIT_PROPERTY( prop, abs_factor, T( 0 ) );
			INIT_PROPERTY( prop, squared_factor, T( 1 ) );
		}

		void Update( TimeInSeconds timestamp, const T& value )
		{
			auto range_violation = range.GetRangeViolation( value );
			auto v = abs_factor * abs( range_violation ) + squared_factor * GetSquared( range_violation );
			penalty.AddSample( timestamp, range_violation );
		}

		T Get() { return penalty.GetAverage(); }

		virtual ~RangePenalty() {}
		
	private:
		Real abs_factor;
		Real squared_factor;

		Range< T > range;
		Statistic< T > penalty;
	};
}
