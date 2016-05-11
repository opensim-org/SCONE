#pragma once

#include "scone/core/Range.h"

namespace scone
{
	template< typename T >
	class Penalty
	{
	public:
		Penalty( const PropNode& prop )
		{
			INIT_PROPERTY_NAMED( prop, range.min, "min", T( 0 ) );
			INIT_PROPERTY_NAMED( prop, range.max, "max", T( 0 ) );
			INIT_PROPERTY( prop, factor, Real( 1 ) );
		}

		void Update( TimeInSeconds timestamp, const T& value )
		{
			auto range_violation = range.GetRangeViolation( value );
			auto v = abs_factor * abs( range_violation ) + squared_factor * GetSquared( range_violation );
			penalty.AddSample( timestamp, range_violation );
		}

		T Get() { return penalty.GetAverage(); }

		virtual ~Penalty() {}
		
	private:
		Real abs_factor;
		Real squared_factor;

		Range< T > range;
		Statistic< T > penalty;
	};
}
