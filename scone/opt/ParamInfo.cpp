#include "stdafx.h"
#include "ParamInfo.h"

#include "Rng/GlobalRng.h"

namespace scone
{
	namespace opt
	{

		ParamInfo::ParamInfo( const String& i_name, double i_init_mean, double i_init_std, double i_init_min, double i_init_max, double i_min, double i_max ) :
		name( i_name ),
		mean( i_init_mean ), std( i_init_std ),
		init_min( i_init_min ),	init_max( i_init_max ),
		min( i_min ), max( i_max ),	is_free( true )
		{
		}

		ParamInfo::ParamInfo( const String& i_name, const PropNode& props )
		{
			SCONE_THROW_NOT_IMPLEMENTED;
		}

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
