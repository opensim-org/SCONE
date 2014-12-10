#pragma once

#include "..\core\core.h"
#include "opt.h"

namespace scone
{
	namespace opt
	{
		class OPT_API Param
		{
		public:
			Param( ) { };
			Param( const String& i_name, double i_val = 0.0, double i_var = 0.0, double i_min = REAL_MIN, double i_max = REAL_MAX ) :
				name( i_name ), init_mean( i_val ), init_var( i_var ), min( i_min ), max( i_max ) { };

			String name;
			double init_mean;
			double init_var;
			double min;
			double max;
		};
	}
}
