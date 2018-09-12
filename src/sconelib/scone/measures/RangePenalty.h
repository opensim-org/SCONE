#pragma once

#include "scone/core/math.h"
#include "scone/core/Range.h"
#include "scone/core/Statistic.h"

namespace scone
{
	/// Helper class to compute penalty if a value is outside a specific range.
	template< typename T > class RangePenalty
	{
	public:
		RangePenalty( const PropNode& prop )
		{
			INIT_PROP_NAMED( prop, range.min, "min", T( 0 ) );
			INIT_PROP_NAMED( prop, range.max, "max", T( 0 ) );
			INIT_PROP( prop, abs_range_penalty, T( 0 ) );
			INIT_PROP( prop, squared_range_penalty, T( 0 ) );
		}

		void AddSample( TimeInSeconds timestamp, const T& value )
		{
			auto range_violation = range.GetRangeViolation( value );
			auto pen = abs_range_penalty * abs( range_violation ) + squared_range_penalty * GetSquared( range_violation );
			penalty.AddSample( timestamp, pen );
		}

		/// Range, set through parameters 'min' and 'max'.
		Range< T > range;

		/// Absolute penalty factor when value is out of range; default = 0.
		Real abs_range_penalty;

		/// Squared penalty factor when value out of range; default = 0.
		Real squared_range_penalty;

		T GetAverage() const { return penalty.GetAverage(); }
		T GetLatest() const { return penalty.GetLatest(); }

		virtual ~RangePenalty() {}
		
	private:
		Statistic< T > penalty;
	};
}
