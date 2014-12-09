#pragma once

#include "..\core\PropNode.h"

using namespace scone;

void XmlParseTest()
{
	PropNode prop;
	prop.FromXmlFile( "d:/optimization_test.xml" );
	prop.ToInfoFile( "d:/optimization_test.info" );
	std::cout << prop;
}
