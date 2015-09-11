#include "stdafx.h"
#include "MetaReflexParams.h"

namespace scone
{
	namespace cs
	{
		MetaReflexParams::MetaReflexParams( const PropNode& props, opt::ParamSet& par, sim::Model& model, const String& prefix /*= "" */ )
		{
			opt::ScopedParamSetPrefixer prefixer( par, prefix );

			ref_pos = Degree( par.TryGet( "ref", props, "ref", 0.0 ) );

			bool use_length = model.GetCustomProp( "meta_reflex_control.use_length", true );
			bool use_force = model.GetCustomProp( "meta_reflex_control.use_force", true );
			bool use_constant = model.GetCustomProp( "meta_reflex_control.use_constant", true );
			bool use_stiffness = model.GetCustomProp( "meta_reflex_control.use_stiffness", true );

			length_gain = use_length ? par.TryGet( "len", props, "len", 0.0 ) : 0;
			force_gain = use_force ? par.TryGet( "for", props, "for", 0.0 ) : 0;
			constant = use_constant ? par.TryGet( "con", props, "con", 0.0 ) : 0;
			stiffness = use_stiffness ? par.TryGet( "imp", props, "imp", 0.0 ) : 0;
		}
	}
}
