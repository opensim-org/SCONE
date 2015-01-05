#include "stdafx.h"
#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Muscle.h"
#include "boost\foreach.hpp"
#include <algorithm>

namespace scone
{
	namespace sim
	{
		Model::Model() :
		m_ShouldTerminate( false )
		{
		}
		
		Model::~Model()
		{
		}

		void Model::ProcessProperties( const PropNode& props )
		{
			InitFromPropNode( props.GetChild( "Controllers" ), m_Controllers );
		}

		void Model::ProcessParameters( opt::ParamSet& par )
		{
			BOOST_FOREACH( ControllerUP& c, m_Controllers )
				c->ProcessParameters( par );
		}

		Body& Model::FindBody( const String& name )
		{
			auto it = std::find_if( m_Bodies.begin(), m_Bodies.end(), [&]( BodyUP& body ) { return body->GetName() == name; } );
			if ( it == m_Bodies.end() )
				SCONE_THROW( "Could not find body: " + name );

			return **it;
		}

	}
}
