/*
** qt_tools.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include <QString>
#include <string>
#include "xo/filesystem/path.h"
#include "xo/string/string_cast.h"
#include "simvis/color.h"

inline QString make_qt( const std::string& s ) { return QString( s.c_str() ); }
inline QString make_qt( const xo::path& s ) { return QString( s.str().c_str() ); }
inline QColor make_qt( const vis::color& c ) { return QColor( 255 * c.r, 255 * c.g, 255 * c.b ); }

namespace xo {
	template<> struct string_cast<QString, void> {
		static QString from( const string& value ) { return QString( value.c_str() ); }
		static string to( const QString& s ) { return s.toStdString(); }
	};
}
