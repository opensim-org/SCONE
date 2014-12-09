#include "stdafx.h"
#include "factory.h"
#include "Optimization.h"
#include "..\core\PropNode.h"

namespace scone
{
	namespace cs
	{
		Optimization::Optimization()
		{
		}

		Optimization::~Optimization()
		{
		}

		void Optimization::Run( const String& script_file )
		{
			PropNode prop;
			prop.FromXmlFile( script_file );
			ProcessProperties( prop );
		}

		void Optimization::ProcessProperties( const PropNode& props )
		{
			ProcessProperty( props, m_Objective, "Objective" );
			ProcessProperty( props, m_Test, "Test" );
			ProcessProperty( props, m_Optimizer, "Optimizer" );
		}
	}
}
