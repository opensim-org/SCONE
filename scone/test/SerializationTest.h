#pragma once

#include "../core/PropNode.h"
#include "../core/serialization.h"

#include <conio.h>
#include <iostream>

using namespace scone;

struct TestStructA
{
	TestStructA() : a( 1.1 ), soepkip(67) { integers.push_back( -999 ); }

	double a;
	int soepkip;
	std::vector< int > integers;
	std::string str;

	void test() { };

	SCONE_PROCESS_DATA_MEMBERS
	{
		SCONE_PROCESS_DATA_MEMBER( a );
		SCONE_PROCESS_DATA_MEMBER( soepkip );
		SCONE_PROCESS_DATA_MEMBER_NAMED( integers, "array_of_integers" );
		SCONE_PROCESS_DATA_MEMBER( str );
	}
};

SCONE_ENABLE_SERIALIZATION( TestStructA );

struct TestStructB
{
	TestStructB() : floep(100) { }

	int floep;
	TestStructA stuff;
	std::vector< TestStructA > stuff_vec;

	void test() { };

	SCONE_PROCESS_DATA_MEMBERS
	{
		SCONE_PROCESS_DATA_MEMBER( floep );
		SCONE_PROCESS_DATA_MEMBER( stuff );
		SCONE_PROCESS_DATA_MEMBER( stuff_vec );
	}
};

SCONE_ENABLE_SERIALIZATION( TestStructB );

void SerializationTest()
{
	TestStructB tsb;
	tsb.floep = 1000;
	tsb.stuff.a = 999.1;
	tsb.stuff.soepkip = -1234;
	tsb.stuff.str = "A New String";

	tsb.stuff_vec.resize( 2 );
	tsb.stuff_vec[1].a = 2.2;
	tsb.stuff_vec[0].str = "First String";
	tsb.stuff_vec[1].str = "Second String";

	for (int i = 1; i <= 5; ++i) tsb.stuff_vec[1].integers.push_back(i * i);

	PropNode prop1;
	WriteData( prop1, tsb );
	prop1.ToXmlFile( "d:/test.xml", "XML_ROOT" );

	PropNode prop2;
	prop2.FromXmlFile( "d:/test.xml", "XML_ROOT" );

	TestStructB tsb2;
	ReadData( prop2, tsb2 );

	PropNode prop3;
	WriteData( prop3, tsb2 );
	std::cout << prop3;

	prop3.ToInfoFile( "d:/test.info" );

	PropNode prop4;
	prop4.FromInfoFile( "d:/test.info" );

	std::cout << (prop1 == prop4) << std::endl;
	std::cout << (prop1 == prop2) << std::endl;
	std::cout << (prop3 == prop4) << std::endl;
	//std::cout << prop4;
}
