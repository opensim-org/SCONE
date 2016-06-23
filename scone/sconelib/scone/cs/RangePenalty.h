#pragma once

#include "scone/core/math.h"
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
			INIT_PROPERTY( prop, abs_range_penalty, T( 0 ) );
			INIT_PROPERTY( prop, squared_range_penalty, T( 0 ) );
		}

		void AddSample( TimeInSeconds timestamp, const T& value )
		{
			auto range_violation = range.GetRangeViolation( value );
			auto pen = abs_range_penalty * abs( range_violation ) + squared_range_penalty * GetSquared( range_violation );
			penalty.AddSample( timestamp, pen );
		}

		T GetAverage() { return penalty.GetAverage(); }
		T GetLatest() { return penalty.GetLatest(); }

		virtual ~RangePenalty() {}
		
	private:
		Real abs_range_penalty;
		Real squared_range_penalty;

		Range< T > range;
		Statistic< T > penalty;
	};
}
