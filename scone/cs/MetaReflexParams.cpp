#include "stdafx.h"
#include "MetaReflexParams.h"

namespace scone
{
	namespace cs
	{
		MetaReflexParams::MetaReflexParams( const PropNode& props, opt::ParamSet& par, sim::Model& model )
		{
			const PropNode& mrcprops = model.GetCustomProps().TryGetChild( "meta_reflex_control" );

			ref_pos = mrcprops.Get( "use_ref", true ) ? Degree( par.TryGet( "ref", props, "ref", 0.0 ) ) : Degree( 0 );
			length_gain = mrcprops.Get( "use_length", true ) ? par.TryGet( "len", props, "len", 0.0 ) : 0;
			force_gain = mrcprops.Get( "use_force", true ) ? par.TryGet( "for", props, "for", 0.0 ) : 0;
			constant = mrcprops.Get( "use_constant", true ) ? par.TryGet( "con", props, "con", 0.0 ) : 0;
			stiffness = mrcprops.Get( "use_stiffness", true ) ? par.TryGet( "imp", props, "imp", 0.0 ) : 0;
			symmetry = mrcprops.Get( "use_symmetry", true ) ? par.TryGet( "sym", props, "sym", 0.0 ) : 0;
		}
	}
}
