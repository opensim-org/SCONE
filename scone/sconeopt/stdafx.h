#pragma once

#if _MSC_VER

#include <SDKDDKVer.h>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <tchar.h>

#else

#include <ncurses.h>

#endif

#include <stdio.h>
#include <string>
#include <vector>
