#pragma once

#include <boost/predef.h>

#include "xo/string/string_tools.h"
using xo::stringf;

#if BOOST_ARCH_X86
#	define SCONE_ARCH "X86"
#else
#	define SCONE_ARCH "UnknownArch"
#endif

#if defined( _MSC_VER )
#	define SCONE_COMP stringf( "MSVC%d", _MSC_VER )
#elif defined( __clang__ )
#	define SCONE_COMP "Clang"
#elif defined( __GNUC__ )
#	define SCONE_COMP "GCC"
#else 
#	define SCONE_COMP "UnknownComp"
#endif

std::string make_platform_id()
{
	return std::string( SCONE_ARCH ) + "-" + std::string( SCONE_COMP );
}


