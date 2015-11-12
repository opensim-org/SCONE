// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _MSC_VER

#include <SDKDDKVer.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>

#else

#include <stdarg.h>
#include <stdio.h>

// enable LARGE_INTEGER
#include <stdint.h>
typedef uint8_t BYTE;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef int64_t LONGLONG;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    };
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

#endif

#include <string>
#include <vector>
#include <algorithm>


//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/ini_parser.hpp>
//#include <boost/property_tree/xml_parser.hpp>
//#include <boost/property_tree/info_parser.hpp>
//#include <boost/tokenizer.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/foreach.hpp>

