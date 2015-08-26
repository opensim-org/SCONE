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
		void MetaReflexDof::DofParams::InitFromPropNode( const PropNode& props, opt::ParamSet& par, sim::Model& model )
		{
			ref_pos_in_deg = Degree( par.Get( "ref", props.GetChild( "ref" ) ) );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_length", true ) )
				INIT_PARAM_NAMED( props, par, length_gain, "len", 0.0 );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_constant", true ) )
				INIT_PARAM_NAMED( props, par, constant, "con", 0.0 );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_force", true ) )
				INIT_PARAM_NAMED( props, par, force_feedback, "for", 0.0 );

			if ( model.custom_properties.GetBool( "meta_reflex_control.use_stiffness", true ) )
				INIT_PARAM_NAMED( props, par, stiffness, "imp", 0.0 );
		}

		MetaReflexDof::MetaReflexDof( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		target_dof( *FindByName( model.GetDofs(), props.GetStr( "target" ) + GetSideName( area.side ) ) ),
		tot_available_pos_mom( 0.0 ),
		tot_available_neg_mom( 0.0 ),
		dof_par(),
		reg_par()
		{
			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "MetaReflex" );
			opt::ScopedParamSetPrefixer prefixer( par, props.GetStr( "target" ) + "." );

			dof_par.InitFromPropNode( props, par, model );

			INIT_PROPERTY_REQUIRED( props, delay );

			// Read in regulate
			if ( props.HasKey( "regulate" ) )
			{
				const PropNode& rp = props.GetChild( "regulate" );
				reg_sensor = &model.AcquireDelayedSensor( rp.GetChild( "Sensor" ), par, area );
				reg_par.InitFromPropNode( props, par, model );
			}
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
	}
}
