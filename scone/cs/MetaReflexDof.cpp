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
		target_dof( *FindByName( model.GetDofs(), props.GetStr( "target" ) + GetSideName( area.side ) ) )
		{
			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "MetaReflex" );

			opt::ScopedParamSetPrefixer prefixer( par, props.GetStr( "target" ) + "." );

			// TODO: fix hacky Degree / Rad conversion!
			ref_pos_in_deg = Degree( par.Get( "reference_pos", props.GetChild( "reference_pos" ) ) );
			ref_pos_in_rad = DegToRad( ref_pos_in_deg );

			INIT_PARAM( props, par, length_gain, 0.0 );
			INIT_PARAM( props, par, constant, 0.0 );
			INIT_PARAM( props, par, force_feedback, 0.0 );
			INIT_PARAM( props, par, stiffness, 0.0 );

			INIT_PROPERTY_REQUIRED( props, delay );
		}

		MetaReflexDof::~MetaReflexDof()
		{
		}

		bool MetaReflexDof::MuscleCrossesDof( const sim::Muscle& mus )
		{
			// see if the muscle passes a joint that contains our dof
			const sim::Link& orgLink = mus.GetOriginLink();
			const sim::Link& insLink = mus.GetInsertionLink();

			// ignore bi-articulair muscles!
			//if ( &insLink.GetParent() != &orgLink )
			//{
			//	log::Trace( "Ignoring muscle " + mus.GetName() + " origin=" + orgLink.GetBody().GetName()
			//		+ " insertion=" + insLink.GetBody().GetName()
			//		+ " parent=" + insLink.GetParent().GetBody().GetName() );
			//	return false;
			//}

			const sim::Link* l = &insLink;
			while ( l && l != &orgLink )
			{
				if ( l->GetJoint().HasDof( target_dof.GetName() ) )
					return true;
				l = &l->GetParent();
			}
			return false;
		}
	}
}
