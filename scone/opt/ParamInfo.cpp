#include "stdafx.h"
#include "ParamInfo.h"

#include "Rng/GlobalRng.h"

namespace scone
{
	namespace opt
	{
		double ParamInfo::GetRandomValue() const
		{
			double value = 0.0;
			if ( init_min == 0.0 && init_max == 0.0 )
			{
				value = Rng::gauss( mean, Square( std ) );
				RestrainValue( value );
			}
			else
			{
				value = Rng::uni( init_min, init_max );
			}
			return value;
		}
	}
}
