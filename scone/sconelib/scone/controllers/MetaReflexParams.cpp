#include "MetaReflexParams.h"

namespace scone
{
	MetaReflexParams::MetaReflexParams( const PropNode& props, Params& par, Model& model, const String& pnp )
	{
		Init( props, par, model, pnp );
	}

	void MetaReflexParams::Init( const PropNode& props, Params& par, Model& model, const String& pnp )
	{
		if ( const PropNode* mrcprops = model.GetCustomProps().try_get_child( "meta_reflex_control" ) )
		{
			ref_pos = mrcprops->get( "use_ref", true ) ? Degree( par.get_or( "R" + pnp, props.try_get_child( "R" + pnp ), 0.0 ) ) : Degree( 0 );
			length_gain = mrcprops->get( "use_length", true ) ? par.get_or( "L" + pnp, props.try_get_child( "L" + pnp ), 0.0 ) : 0;
			force_gain = mrcprops->get( "use_force", true ) ? par.get_or( "F" + pnp, props.try_get_child( "F" + pnp ), 0.0 ) : 0;
			constant = mrcprops->get( "use_constant", true ) ? par.get_or( "C" + pnp, props.try_get_child( "C" + pnp ), 0.0 ) : 0;
			stiffness = mrcprops->get( "use_stiffness", true ) ? par.get_or( "S" + pnp, props.try_get_child( "S" + pnp ), 0.0 ) : 0;
			symmetry = mrcprops->get( "use_symmetry", true ) ? par.get_or( "sym" + pnp, props.try_get_child( "sym" + pnp ), 0.0 ) : 0;
		}
	}
}
