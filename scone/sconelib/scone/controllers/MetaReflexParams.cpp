#include "MetaReflexParams.h"

namespace scone
{
	MetaReflexParams::MetaReflexParams( const PropNode& props, ParamSet& par, Model& model, const String& pnp )
	{
		Init( props, par, model, pnp );
	}

	void MetaReflexParams::Init( const PropNode& props, ParamSet& par, Model& model, const String& pnp )
	{
		if ( const PropNode* mrcprops = model.GetCustomProps().try_get_child( "meta_reflex_control" ) )
		{
			ref_pos = mrcprops->get( "use_ref", true ) ? Degree( par.Get( "R" + pnp, props, "R" + pnp, 0.0 ) ) : Degree( 0 );
			length_gain = mrcprops->get( "use_length", true ) ? par.Get( "L" + pnp, props, "L" + pnp, 0.0 ) : 0;
			force_gain = mrcprops->get( "use_force", true ) ? par.Get( "F" + pnp, props, "F" + pnp, 0.0 ) : 0;
			constant = mrcprops->get( "use_constant", true ) ? par.Get( "C" + pnp, props, "C" + pnp, 0.0 ) : 0;
			stiffness = mrcprops->get( "use_stiffness", true ) ? par.Get( "S" + pnp, props, "S" + pnp, 0.0 ) : 0;
			symmetry = mrcprops->get( "use_symmetry", true ) ? par.Get( "sym" + pnp, props, "sym" + pnp, 0.0 ) : 0;
		}
	}
}
