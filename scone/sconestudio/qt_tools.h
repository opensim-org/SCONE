#pragma once

#include "QString"
#include <string>

inline QString make_qt( const std::string& s ) { return QString( s.c_str() ); }