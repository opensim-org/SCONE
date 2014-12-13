#include "stdafx.h"
#include "Optimization.h"
#include "..\core\PropNode.h"

namespace scone
{
	namespace opt
	{
		Optimization::Optimization()
		{
			RegisterFactoryTypes();
		}

		Optimization::~Optimization()
		{
		}

		void Optimization::Run( const String& script_file )
		{
			PropNode prop;
			prop.FromXmlFile( script_file );
			ProcessProperty( prop, *this, "Optimization" );

			m_Optimizer->Run();
		}

		void Optimization::ProcessProperties( const PropNode& props )
		{
			ProcessProperty( props, m_Optimizer, "Optimizer" );
		}
	}
}
