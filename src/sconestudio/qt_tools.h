#pragma once

#include <QString>
#include <string>
#include "xo/filesystem/path.h"
#include "xo/string/string_cast.h"

inline QString make_qt( const std::string& s ) { return QString( s.c_str() ); }
inline QString make_qt( const xo::path& s ) { return QString( s.str().c_str() ); }

namespace xo {
	template<> struct string_cast<QString, void> {
		static QString from( const string& value ) { return QString( value.c_str() ); }
		static string to( const QString& s ) { return s.toStdString(); }
	};
}
