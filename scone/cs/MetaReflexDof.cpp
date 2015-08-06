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
		ref_pos_in_deg( 0.0 ),
		length_gain( 0.0 ),
		constant( 0.0 ),
		force_feedback( 0.0 ),
		stiffness( 0.0 )
		{
			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "MetaReflex" );
			opt::ScopedParamSetPrefixer prefixer( par, props.GetStr( "target" ) + "." );

			ref_pos_in_deg = Degree( par.Get( "reference_pos", props.GetChild( "reference_pos" ) ) );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_length", true ) )
				INIT_PARAM( props, par, length_gain, 0.0 );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_constant", true ) )
				INIT_PARAM( props, par, constant, 0.0 );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_force", true ) )
				INIT_PARAM( props, par, force_feedback, 0.0 );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_stiffness", true ) )
				INIT_PARAM( props, par, stiffness, 0.0 );

			INIT_PROPERTY_REQUIRED( props, delay );
		}

		MetaReflexDof::~MetaReflexDof()
		{
		}
	}
}
