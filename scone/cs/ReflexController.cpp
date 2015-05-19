#include "stdafx.h"

#include "ReflexController.h"
#include "Reflex.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>
#include "../sim/Model.h"
#include "tools.h"
#include "../sim/Area.h"
#include "../core/Profiler.h"

namespace scone
{
	namespace cs
	{
		ReflexController::ReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area ) :
		sim::Controller( props, par, model, target_area )
		{
			bool symmetric = props.GetBool( "use_symmetric_actuators", true );
			SCONE_ASSERT( symmetric == true ); // only symmetric controllers work for now

			// initialize monosynaptic reflexes
			const PropNode& msr = props.GetChild( "MonoSynapticReflexes" );
			BOOST_FOREACH( const PropNode::KeyChildPair& item, msr.GetChildren() )
			{
				// extract individual muscle names from "targets"
				String targets = item.second->GetStr( "targets" );
				boost::char_separator< char > separator(", ");
				boost::tokenizer< boost::char_separator< char > > tokens( targets, separator );
				BOOST_FOREACH( String musname, tokens )
				{
					if ( symmetric && GetSide( musname ) == NoSide )
						musname += GetSideName( target_area.side ); // make sure the muscle has a sided name

					// find muscle
					sim::Muscle& m = FindNamed( model.GetMuscles(), musname );
					opt::ScopedParamSetPrefixer prefixer( par, ( symmetric ? GetNameNoSide( musname ) : musname ) + "." );
					m_Reflexes.push_back( ReflexUP( new Reflex( *item.second, par, model, m, m ) ) );
				}

				// create others
			}
		}

		ReflexController::~ReflexController()
		{
		}

		sim::Controller::UpdateResult ReflexController::UpdateControls( sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			// IMPORTANT: delayed storage must have been updated in through Model::UpdateSensorDelayAdapters()
			BOOST_FOREACH( ReflexUP& r, m_Reflexes )
				r->ComputeControls( timestamp );

			return SuccessfulUpdate;
		}

		scone::String ReflexController::GetMainSignature() const
		{
			String str = "R"; //GetStringF( "r%d", m_Reflexes.size() );

			// count reflex types
			int l = 0, v = 0, f = 0;
			BOOST_FOREACH( const ReflexUP& r, m_Reflexes )
			{
				if ( r->length_gain != 0.0 ) ++l;
				if ( r->velocity_gain != 0.0 ) ++v;
				if ( r->force_gain != 0.0 ) ++f;
			}

			if ( l > 0 ) str += "L";
			if ( v > 0 ) str += "V";
			if ( f > 0 ) str += "F";

			return str;
		}
	}
}
