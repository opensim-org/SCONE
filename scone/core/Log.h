#pragma once

#include <iostream>

// very basic logging
#define SCONE_LOG( _MESSAGE_ ) std::cout << _MESSAGE_ << std::endl;

namespace scone
{
	static std::ostream& Log = std::cout;
	static const char* NewLine = "\n";
}