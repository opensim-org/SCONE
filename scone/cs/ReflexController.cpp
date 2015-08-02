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
			bool symmetric = props.GetBool( "use_symmetric_actuators", true );
			SCONE_ASSERT( symmetric == true ); // only symmetric controllers work for now
			String sidename = GetSideName( area.side );

			// create normal reflexes
			const PropNode& reflexvec = props.GetChild( "Reflexes" );
			BOOST_FOREACH( const PropNode::KeyChildPair& item, reflexvec.GetChildren() )
			{
				m_Reflexes.push_back( CreateReflex( *item.second, par, model, area ) );
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

		scone::String ReflexController::GetClassSignature() const
		{
			//String str = GetStringF( "r%d", m_Reflexes.size() );

			// count reflex types
			//int l = 0, v = 0, f = 0;
			//BOOST_FOREACH( const MuscleReflexUP& r, m_Reflexes )
			//{
			//	if ( r->length_gain != 0.0 ) ++l;
			//	if ( r->velocity_gain != 0.0 ) ++v;
			//	if ( r->force_gain != 0.0 ) ++f;
			//}

			//if ( l > 0 ) str += "L";
			//if ( v > 0 ) str += "V";
			//if ( f > 0 ) str += "F";

			return "R";
		}
	}
}
