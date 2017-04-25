#include "ParamInfo.h"

#include "opt_config.h"

#if SHARK_VERSION == 2
#include "Rng/GlobalRng.h"
#endif

#include "scone/core/propnode_tools.h"

namespace scone
{
	ParamInfo::ParamInfo( const String& i_name, double i_init_mean, double i_init_std, double i_init_min, double i_init_max, double i_min, double i_max ) :
		name( i_name ),
		init_mean( i_init_mean ), init_std( i_init_std ),
		init_min( i_init_min ), init_max( i_init_max ),
		min( i_min ), max( i_max ), is_free( true )
	{
	}

	ParamInfo::ParamInfo( const String& i_name, const PropNode& props ) :
		name( i_name )
	{
		INIT_PROPERTY( props, init_min, 0.0 );
		INIT_PROPERTY( props, init_max, 0.0 );
		INIT_PROPERTY( props, init_mean, props.has_value() ? props.get<double>() : 0.0 );
		INIT_PROPERTY( props, init_std, 0.0 );
		INIT_PROPERTY( props, min, -1e9 ); // don't use REAL_MIN to prevent boundary issues
		INIT_PROPERTY( props, max, 1e9 ); // don't use REAL_MAX to prevent boundary issues
		INIT_PROPERTY( props, is_free, props.has_value() ? false : true );
	}

	double ParamInfo::GetInitialValue() const
	{
#if SHARK_VERSION == 2
		if ( is_free )
		{
			if ( init_mean != 0.0 || init_std != 0.0 )
				return GetRestrained( Rng::gauss( init_mean, GetSquared( init_std ) ), min, max );
			else
				return Rng::uni( init_min, init_max );
		}
		else return init_mean;
#endif
		// this function cannot be used with Shark 3
		return init_mean;
	}
}
