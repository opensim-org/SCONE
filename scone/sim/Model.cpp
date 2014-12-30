#include "stdafx.h"
#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Muscle.h"
#include "boost\foreach.hpp"

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

		void Model::InitControllers()
		{
			BOOST_FOREACH( ControllerUP& c, m_Controllers )
				c->Initialize( *this );
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

		void Model::Reset()
		{
			BOOST_FOREACH( ControllerUP& c, m_Controllers )
				c->Initialize( *this );
		}
	}
}
