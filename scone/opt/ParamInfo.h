#pragma once

#include "../core/core.h"
#include "opt.h"
#include "../core/math.h"

namespace scone
{
	namespace opt
	{
		// TODO: rename to Param and store value as well. Why not?
		struct OPT_API ParamInfo
		{
			ParamInfo( const String& i_name, double i_init_mean, double i_init_std, double i_init_min, double i_init_max, double i_min, double i_max );
			ParamInfo( const String& i_name, const PropNode& props );

			/// public member variables
			String name;
			double mean;
			double std;
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
