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
				value( i_val ), variance( i_var ), min( i_min ), max( i_max ) { };

			double value;
			double variance;
			double min;
			double max;
		};
	}
}
