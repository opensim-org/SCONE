#include "stdafx.h"

#include "MetaReflexController.h"

#include <boost/foreach.hpp>

#include "../sim/Area.h"
#include "../sim/Model.h"
#include "../sim/Dof.h"
#include "../core/HasName.h"

#include "tools.h"
#include "Factories.h"
#include "MetaReflex.h"

namespace scone
{
	namespace cs
	{
		MetaReflexController::MetaReflexController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Controller( props, par, model, area )
		{
			bool symmetric = props.GetBool( "use_symmetric_actuators", true );
			SCONE_ASSERT( symmetric == true ); // only symmetric controllers work for now
			//String sidename = GetSideName( area.side );

			// automatically create controllers for all legs (sides)
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
					m_Reflexes.push_back( MetaReflexUP( new MetaReflex( *item.second, par, model, sim::Area::WHOLE_BODY ) ) );
				}
				else
				{
					// this is a dof that has sides (probably), create a controller for each leg
					for ( size_t legIdx = 0; legIdx < model.GetLegs().size(); ++legIdx )
					{
						sim::Area a = model.GetLeg( legIdx ).GetSide() == LeftSide ? sim::Area::LEFT_SIDE : sim::Area::RIGHT_SIDE;
						m_Reflexes.push_back( MetaReflexUP( new MetaReflex( *item.second, par, model, a ) ) );
					}
				}
			}
		}

		MetaReflexController::~MetaReflexController()
		{
		}
	}
}
