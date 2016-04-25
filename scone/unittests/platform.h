#pragma once

#include <boost/predef.h>

#include "flut/string_tools.hpp"
using flut::stringf;

#if BOOST_ARCH_X86
#	define SCONE_ARCH "X86"
#else
#	define SCONE_ARCH "UnknownArch"
#endif

#if defined( BOOST_COMP_MSVC )
#	define SCONE_COMP stringf( "MSVC%d", _MSC_VER )
#elif defined( BOOST_COMP_CLANG )
#	define SCONE_COMP "Clang"
#else
#	define SCONE_COMP "UnknownComp"
#endif

std::string make_platform_id()
{
	return std::string( SCONE_ARCH ) + "-" + std::string( SCONE_COMP );
}


