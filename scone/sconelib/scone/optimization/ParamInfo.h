#pragma once

#include "scone/core/core.h"
#include "scone/core/math.h"
#include "scone/core/PropNode.h"

namespace scone
{
	// TODO: rename to Param and store value as well. Why not?
	// One possible reason: we only need these for optimization, but not for the end result
	// But does that matter? we can throw the ParamSet away after a controller has been constructed
	// Still, it may be better to keep only metadata in this class
	struct SCONE_API ParamInfo
	{
		ParamInfo( const String& i_name, double i_init_mean, double i_init_std, double i_init_min, double i_init_max, double i_min, double i_max );
		ParamInfo( const String& i_name, const PropNode& props );

		/// param id
		String name;

		/// current state
		double init_mean;
		double init_std;

		/// initialization options
		double init_min;
		double init_max;

		/// parameter properties
		double min;
		double max;
		bool is_free;

		bool CheckValue( const double& value ) const { return value >= min && value <= max; }
		void ClampValue( double& value ) const { Restrain( value, min, max ); }
		double GetInitialValue() const;
	};
}
