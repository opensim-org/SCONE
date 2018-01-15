#pragma once

#include <QString>
#include <string>
#include "xo/filesystem/path.h"

inline QString make_qt( const std::string& s ) { return QString( s.c_str() ); }
inline QString make_qt( const xo::path& s ) { return QString( s.str().c_str() ); }
