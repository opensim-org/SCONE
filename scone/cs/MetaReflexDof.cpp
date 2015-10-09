#include "stdafx.h"

#include <boost/foreach.hpp>

#include "MetaReflexDof.h"
#include "../sim/Area.h"
#include "../sim/Dof.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Muscle.h"
#include "../sim/Link.h"
#include "../sim/Joint.h"
#include "../sim/SensorDelayAdapter.h"
#include "tools.h"

//#define DEBUG_MUSCLE "hamstrings_r"

namespace scone
{
	namespace cs
	{
		MetaReflexDof::MetaReflexDof( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		target_dof( *FindByName( model.GetDofs(), props.GetStr( "target" ) + GetSideName( area.side ) ) ),
		tot_available_pos_mom( 0.0 ),
		tot_available_neg_mom( 0.0 ),
		dof_par(),
		bal_par(),
		local_balance( 0 )
		{
			INIT_PROPERTY( props, target_dir, BothDirs );

			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "MetaReflex" );

			const char* tdpostfix = target_dir == PositiveDir ? "+" : ( target_dir == NegativeDir ? "-" : "" );
			opt::ScopedParamSetPrefixer prefixer( par, props.GetStr( "target" ) + tdpostfix + "." );

			dof_par = MetaReflexParams( props, par, model );
			if ( model.GetCustomProp( "meta_reflex_control.use_balance", true ) && props.HasKey( "Balance" ) )
			{
				opt::ScopedParamSetPrefixer pre2( par, "B." );
				bal_par = MetaReflexParams( props.GetChild( "Balance" ), par, model );
			}

			// TODO: move to muscle
			INIT_PROPERTY_REQUIRED( props, delay );
		}

		MetaReflexDof::~MetaReflexDof()
		{
		}

		void MetaReflexDof::AddAvailableMoment( Real max_moment )
		{
			if ( max_moment > 0 )
				tot_available_pos_mom += max_moment;
			else tot_available_neg_mom += max_moment; // neg moment stays negative
		}

		void MetaReflexDof::UpdateLocalBalance( const Vec3& global_balance )
		{
			local_balance = GetDotProduct( global_balance, dof_rotation_axis );
		}

		scone::Real MetaReflexDof::GetLocalBalance()
		{
			return local_balance;
		}

		void MetaReflexDof::SetDofRotationAxis()
		{
			dof_rotation_axis = target_dof.GetRotationAxis();
		}
	}
}
