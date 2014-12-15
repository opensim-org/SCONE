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

		OptimizerSP CreateOptimizerFromXml( const String& xml_file, const String& key )
		{
			CmaOptimizer::RegisterFactory();

			PropNode p = LoadXmlFile( xml_file );

			OptimizerSP opt;
			ProcessProperty( p, opt, key );

			return opt;
		}
	}
}
