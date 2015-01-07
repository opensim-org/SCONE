#pragma once

#include "../core/PropNode.h"

using namespace scone;

void XmlParseTest()
{
	PropNode prop;
	prop.FromXmlFile( "config/optimization_test.xml" );
	prop.ToInfoFile( "config/optimization_test.info" );
	std::cout << prop;
}
