#include "stdafx.h"

#include "MetaReflexController.h"

#include <boost/foreach.hpp>

#include "../sim/sim.h"
#include "../sim/Area.h"
#include "../sim/Model.h"
#include "../sim/Dof.h"
#include "../sim/Muscle.h"
#include "../core/HasName.h"

#include "tools.h"
#include "Factories.h"
#include "MetaReflexDof.h"
#include "MetaReflexMuscle.h"

namespace scone
{
	namespace cs
	{
		MetaReflexController::MetaReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Controller( props, par, model, area )
		{
			bool symmetric = props.GetBool( "use_symmetric_actuators", true );
			SCONE_ASSERT( symmetric == true ); // only symmetric controllers work for now

			// create Meta Reflexes
			const PropNode& reflexes = props.GetChild( "Reflexes" );
			BOOST_FOREACH( const PropNode::KeyChildPair& item, reflexes.GetChildren() )
			{
				// check if the target dof is sided
				// TODO: see if we can come up with something nicer here...
				const String& target_dof = item.second->GetStr( "target" );
				SCONE_ASSERT( GetSide( target_dof ) == NoSide );
				if ( HasElementWithName( model.GetDofs(), target_dof ) )
				{
					// this is a dof with no sides: only create one controller
					m_ReflexDofs.push_back( MetaReflexDofUP( new MetaReflexDof( *item.second, par, model, sim::Area::WHOLE_BODY ) ) );
				}
				else
				{
					// this is a dof that has sides (probably), create a controller for each leg
					for ( size_t legIdx = 0; legIdx < model.GetLegs().size(); ++legIdx )
					{
						sim::Area a = model.GetLeg( legIdx ).GetSide() == LeftSide ? sim::Area::LEFT_SIDE : sim::Area::RIGHT_SIDE;
						m_ReflexDofs.push_back( MetaReflexDofUP( new MetaReflexDof( *item.second, par, model, a ) ) );
					}
				}
			}

			// now set the DOFs
			auto org_state = model.GetStateValues();
			BOOST_FOREACH( MetaReflexDofUP& mr, m_ReflexDofs )
				model.SetStateVariable( mr->target_dof.GetName(), mr->ref_pos_in_rad );

			// Create meta reflex muscles
			BOOST_FOREACH( sim::MuscleUP& mus, model.GetMuscles() )
				m_ReflexMuscles.push_back( MetaReflexMuscleUP( new MetaReflexMuscle( *mus, model, m_ReflexDofs ) ) );

			// restore original state
			model.SetStateValues( org_state );
		}

		MetaReflexController::~MetaReflexController()
		{
		}

		MetaReflexController::UpdateResult MetaReflexController::UpdateControls( sim::Model& model, double timestamp )
		{
			BOOST_FOREACH( MetaReflexMuscleUP& mrmus, m_ReflexMuscles )
				mrmus->UpdateControls();

			return SuccessfulUpdate;
		}

		scone::String MetaReflexController::GetClassSignature() const 
		{
			// count reflex types
			int l = 0, c = 0, f = 0, s = 0;
			BOOST_FOREACH( const MetaReflexMuscleUP& r, m_ReflexMuscles )
			{
				if ( r->length_gain != 0.0 ) ++l;
				if ( r->constant_ex != 0.0 ) ++c;
				if ( r->force_gain != 0.0 ) ++f;
				if ( r->stiffness != 0.0 ) ++s;
			}

			String str = "MR-";

			if ( l > 0 ) str += "L";
			if ( c > 0 ) str += "C";
			if ( f > 0 ) str += "F";
			if ( s > 0 ) str += "S";

			return str;
		}
	}
}
