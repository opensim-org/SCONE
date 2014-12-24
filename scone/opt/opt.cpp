#include "stdafx.h"
#include "opt.h"
#include "..\core\Factory.h"
#include "CmaOptimizer.h"

namespace scone
{
	namespace opt
	{
		void RegisterFactoryTypes()
		{
			CmaOptimizer::RegisterFactory();
		}

		OptimizerUP CreateOptimizerFromXml( const String& xml_file, const String& key )
		{
			RegisterFactoryTypes();

			PropNode p = LoadXmlFile( xml_file );
			return CreateFromPropNode< Optimizer >( p.GetChild( "Optimizer" ) );
		}
	}
}
