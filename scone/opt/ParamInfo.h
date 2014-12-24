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
			ParamInfo( const String& i_name,
				double i_init_mean,	double i_init_std,
				double i_init_min, double i_init_max,
				double i_min, double i_max ) :
				name( i_name ),
				init_mean( i_init_mean ), init_std( i_init_std ),
				init_min( i_init_min ),	init_max( i_init_max ),
				min( i_min ), max( i_max ),	is_free( true ) { };

			/// public member variables
			String name;
			double init_mean;
			double init_std;
			double init_min;
			double init_max;
			double min;
			double max;
			bool is_free;

			bool CheckValue( const double& value ) const { return value >= min && value <= max; }
			void RestrainValue( double& value ) const { Restrain( value, min, max ); }
			double GetRandomValue() const;
		};
	}
}
