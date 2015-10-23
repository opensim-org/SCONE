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
#include "../core/InitFromPropNode.h"

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
				SCONE_ASSERT( GetSide( target_dof ) == NoSide ); // must be symmetric
				if ( HasElementWithName( model.GetDofs(), target_dof ) )
				{
					// this is a dof with no sides: only create one controller
					m_ReflexDofs.push_back( MetaReflexDofUP( new MetaReflexDof( *item.second, par, model, sim::Area::WHOLE_BODY ) ) );
				}
				else
				{
					// this is a dof that has sides (probably), create a controller that matches the Area side
					SCONE_ASSERT( area.side != NoSide )
					if ( area.side == NoSide )
					{
						for ( size_t legIdx = 0; legIdx < model.GetLegs().size(); ++legIdx )
							m_ReflexDofs.push_back( MetaReflexDofUP( new MetaReflexDof( *item.second, par, model, model.GetLeg( legIdx ).GetArea() ) ) );
					}
					else
					{
						m_ReflexDofs.push_back( MetaReflexDofUP( new MetaReflexDof( *item.second, par, model, area ) ) );
					}
				}
			}

			// backup the current state
			auto org_state = model.GetStateValues();

			// reset all dofs to ensure consistency when there are unspecified dofs
			BOOST_FOREACH( sim::DofUP& dof, model.GetDofs() )
			{
				dof->SetPos( 0, false );
				dof->SetVel( 0 );
			}

			// now set the DOFs
			// TODO: include mirror_left variable!
			BOOST_FOREACH( MetaReflexDofUP& mr, m_ReflexDofs )
				mr->target_dof.SetPos( Radian( Degree( mr->dof_pos.ref_pos ) ), false );

			// set target dof rotation axes (required for local balance)
			BOOST_FOREACH( MetaReflexDofUP& mr, m_ReflexDofs )
				mr->SetDofRotationAxis();

			// Create meta reflex muscles
			BOOST_FOREACH( sim::MuscleUP& mus, model.GetMuscles() )
			{
				if ( GetSide( mus->GetName() ) == area.side )
				{
					MetaReflexMuscleUP mrm = MetaReflexMuscleUP( new MetaReflexMuscle( *mus, model, *this, area ) );
					if ( mrm->dof_infos.size() > 0 ) // only keep reflex if it crosses any of the relevant dofs
						m_ReflexMuscles.push_back( std::move( mrm ) );
				}
			}

			// init meta reflex control parameters
			BOOST_FOREACH( MetaReflexMuscleUP& mrm, m_ReflexMuscles )
				mrm->UpdateMuscleControlParameters();

			// restore original state
			model.SetStateValues( org_state );
		}

		MetaReflexController::~MetaReflexController()
		{
		}

		MetaReflexController::UpdateResult MetaReflexController::UpdateControls( sim::Model& model, double timestamp )
		{
			// get balance
			Vec3 global_balance = model.GetDelayedOrientation();

			BOOST_FOREACH( MetaReflexDofUP& mrdof, m_ReflexDofs )
				mrdof->UpdateLocalBalance( global_balance ); // TODO: perhaps not every time?

			BOOST_FOREACH( MetaReflexMuscleUP& mrmus, m_ReflexMuscles )
			{
				mrmus->UpdateMuscleControlParameters(); // TODO: perhaps not every time?
				mrmus->UpdateControls();
			}

			return SuccessfulUpdate;
		}

		String MetaReflexController::GetClassSignature() const 
		{
			// count reflex types
			int l = 0, c = 0, f = 0, s = 0;
			BOOST_FOREACH( const MetaReflexMuscleUP& r, m_ReflexMuscles )
			{
				if ( r->length_gain != 0.0 ) ++l;
				if ( r->constant != 0.0 ) ++c;
				if ( r->force_gain != 0.0 ) ++f;
				if ( r->stiffness != 0.0 ) ++s;
			}

			String str = "M";
			if ( l > 0 ) str += "L";
			if ( c > 0 ) str += "C";
			if ( f > 0 ) str += "F";
			if ( s > 0 ) str += "S";

			return str;
		}

		void MetaReflexController::StoreData( Storage< Real >::Frame& frame )
		{
			BOOST_FOREACH( MetaReflexDofUP& mr, m_ReflexDofs )
				mr->StoreData( frame );
		}
	}
}
