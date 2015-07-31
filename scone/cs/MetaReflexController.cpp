#include "stdafx.h"

#include "MetaReflexController.h"
#include "../sim/Area.h"
#include "tools.h"
#include <boost/foreach.hpp>
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
			String sidename = GetSideName( area.side );

			// create reflexes
			const PropNode& reflexes = props.GetChild( "Reflexes" );
			BOOST_FOREACH( const PropNode::KeyChildPair& item, reflexes.GetChildren() )
			{
				m_Reflexes.push_back( MetaReflexUP( new MetaReflex( *item.second, par, model, area ) ) );
			}
		}

		MetaReflexController::~MetaReflexController()
		{
		}
	}
}
