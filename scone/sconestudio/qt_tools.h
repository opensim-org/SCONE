#pragma once

#include <QString>
#include <string>
#include "flut/system/path.hpp"

inline QString make_qt( const std::string& s ) { return QString( s.c_str() ); }
inline QString make_qt( const flut::path& s ) { return QString( s.str().c_str() ); }
