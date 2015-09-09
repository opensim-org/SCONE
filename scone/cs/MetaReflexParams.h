#pragma once

namespace scone
{
	namespace cs
	{
		struct MetaReflexParams
		{
			MetaReflexParams() {}
			MetaReflexParams( const PropNode& props, opt::ParamSet& par, sim::Model& model, const String& prefix = "" )
			{
				opt::ScopedParamSetPrefixer prefixer( par, prefix );

				ref_pos = Degree( par.Get( "ref", props.GetChild( "ref" ) ) );

				if ( model.custom_properties.GetBool( "meta_reflex_control.use_length", true ) )
					INIT_PARAM_NAMED( props, par, length_gain, "len", 0.0 );
				else length_gain = 0.0;

				if ( model.custom_properties.GetBool( "meta_reflex_control.use_force", true ) )
					INIT_PARAM_NAMED( props, par, force_gain, "for", 0.0 );
				else force_gain = 0.0;

				if ( model.custom_properties.GetBool( "meta_reflex_control.use_constant", true ) )
					INIT_PARAM_NAMED( props, par, constant, "con", 0.0 );
				else constant = 0.0;

				if ( model.custom_properties.GetBool( "meta_reflex_control.use_stiffness", true ) )
					INIT_PARAM_NAMED( props, par, stiffness, "imp", 0.0 );
				else stiffness = 0.0;
			}

			Real ref_pos;
			Real length_gain;
			Real force_gain;
			Real constant;
			Real stiffness;
		};
	}
}
