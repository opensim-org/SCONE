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
			GetOptimizerFactory().Register< CmaOptimizer >();
		}

		OptimizerUP CreateOptimizerFromXml( const String& xml_file, const String& key )
		{
			RegisterFactoryTypes();

			PropNode p = CreatePropNodeFromXmlFile( xml_file );
			const PropNode& optprops = p.GetChild( "Optimizer" );
			OptimizerUP o = CreateOptimizer( optprops );

			// report unused parameters
			p.ToStream( std::cout, "Unused parameter ", true );

			return o;
		}
	}
}
