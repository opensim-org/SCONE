#include "stdafx.h"

#include "ReflexController.h"

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

#include "../core/InitFromPropNode.h"
#include "../core/Log.h"
#include "../core/Profiler.h"

#include "../sim/sim.h"
#include "../sim/Controller.h"
#include "../sim/Model.h"
#include "../sim/Muscle.h"
#include "../sim/Area.h"

#include "Factories.h"
#include "MuscleReflex.h"
#include "Tools.h"

namespace scone
{
	namespace cs
	{
		ReflexController::ReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Controller( props, par, model, area )
		{
			sim::Actuator& act = model.GetMuscle( 0 );

			bool symmetric = props.GetBool( "use_symmetric_actuators", true );
			SCONE_ASSERT( symmetric == true ); // only symmetric controllers work for now
			String sidename = GetSideName( area.side );

			// initialize monosynaptic reflexes (TODO: get rid of those)
			if ( props.HasKey( "MonoSynapticReflexes" ) )
			{
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
							musname += sidename; // make sure the muscle has a sided name

						// find muscle
						sim::Muscle& muscle = *FindNamed( model.GetMuscles(), musname );
						opt::ScopedParamSetPrefixer prefixer( par, ( symmetric ? GetNameNoSide( musname ) : musname ) + "." );
						m_MuscleReflexes.push_back( MuscleReflexUP( new MuscleReflex( *item.second, par, model, muscle, muscle ) ) );
					}
				}
			}

			// create normal reflexes
			// TODO: move the lot of this code to Reflex constructor(s)
			const PropNode& reflexvec = props.GetChild( "Reflexes" );
			BOOST_FOREACH( const PropNode::KeyChildPair& item, reflexvec.GetChildren() )
			{
				const PropNode& rprops = *item.second;

				String trgname = rprops.GetStr( "target" );
				sim::Actuator& target = *FindNamed( model.GetActuators(), trgname + sidename );

				String srcname = rprops.GetStr( "source", trgname );
				sim::Sensor& source = ContainsNamed( model.GetSensors(), srcname + sidename ) ?
					*FindNamed( model.GetSensors(), srcname + sidename ) : *FindNamed( model.GetSensors(), srcname );
				//Index source_idx = source.GetSensorIndex( rprops.GetStr( "source_channel" ) );

				// get name for this reflex
				String reflexname = GetNameNoSide( target.GetName() );
				if ( source.GetName() != target.GetName() )
					reflexname += "-" + GetNameNoSide( source.GetName() );

				opt::ScopedParamSetPrefixer prefixer( par, reflexname + "." );
				m_Reflexes.push_back( CreateReflex( *item.second, par, model, target, source ) );
			}
		}

		ReflexController::~ReflexController()
		{
		}

		sim::Controller::UpdateResult ReflexController::UpdateControls( sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			// IMPORTANT: delayed storage must have been updated in through Model::UpdateSensorDelayAdapters()
			BOOST_FOREACH( MuscleReflexUP& r, m_MuscleReflexes )
				r->ComputeControls( timestamp );

			return SuccessfulUpdate;
		}

		scone::String ReflexController::GetMainSignature() const
		{
			String str = "R"; //GetStringF( "r%d", m_Reflexes.size() );

			// count reflex types
			int l = 0, v = 0, f = 0;
			BOOST_FOREACH( const MuscleReflexUP& r, m_MuscleReflexes )
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
