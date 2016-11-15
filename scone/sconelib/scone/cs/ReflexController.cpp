#include "ReflexController.h"

#include "scone/core/propnode_tools.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"

#include "scone/sim/Model.h"
#include "scone/sim/Muscle.h"
#include "scone/sim/Area.h"

#include "Factories.h"
#include "MuscleReflex.h"

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
			const PropNode& reflexvec = props.get_child( "Reflexes" );
			for ( const PropNode::KeyChildPair& item: reflexvec.GetChildren() )
			{
				// todo: handle "targets" tag? or create a new class for groups of monosynaptic muscles?
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
			for ( ReflexUP& r: m_Reflexes )
				r->ComputeControls( timestamp );

			return SuccessfulUpdate;
		}

		scone::String ReflexController::GetClassSignature() const
		{
			//String str = stringf( "r%d", m_Reflexes.size() );

			// count reflex types
			//int l = 0, v = 0, f = 0;
			//for ( const MuscleReflexUP& r, m_Reflexes )
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
