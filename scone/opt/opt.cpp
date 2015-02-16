#include "stdafx.h"
#include "opt.h"
#include "CmaOptimizer.h"
#include "Factories.h"

namespace scone
{
	namespace opt
	{
		void RegisterFactoryTypes()
		{
			//TOFIX: CmaOptimizer::RegisterFactory();
		}

		OptimizerUP CreateOptimizerFromXml( const String& xml_file, const String& key )
		{
			RegisterFactoryTypes();

			PropNode p = ReadXmlFile( xml_file );
			const PropNode& optprops = p.GetChild( "Optimizer" );
			OptimizerUP o = CreateOptimizer( optprops );

			// report unused parameters
			p.ToStream( std::cout, "Unused parameter ", true );

			return o;
		}
	}
}
