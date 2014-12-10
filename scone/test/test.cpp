// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SerializationTest.h"
#include "XmlParseTest.h"
#include "OptimizationTest.h"

int _tmain(int argc, _TCHAR* argv[])
{
	scone::OptimizationTest();

	_getch();

	return 0;
}
