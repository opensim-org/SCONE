#pragma once

#include "..\core\core.h"
#include "opt.h"
#include "..\core\math.h"

namespace scone
{
	namespace opt
	{
		struct OPT_API ParamInfo
		{
			ParamInfo( const String& i_name, double i_mean = 0.0, double i_var = 0.0, double i_min = REAL_MIN, double i_max = REAL_MAX ) :
				name( i_name ), init_mean( i_mean ), init_var( i_var ), min( i_min ), max( i_max ), is_free( true ) { };

			String name;
			double init_mean;
			double init_var;
			double min;
			double max;
			bool is_free;

			bool CheckValue( const double& value ) const { return value >= min && value <= max; }
			void RestrainValue( double& value ) { Restrain( value, min, max ); }
		};
	}
}
