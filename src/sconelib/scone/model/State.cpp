/*
** State.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "State.h"

#include <algorithm>
#include "scone/core/Exception.h"

namespace scone
{
	index_t State::AddVariable( const String& name, Real value )
	{
		names_.push_back( name );
		values_.push_back( value );
		return names_.size() - 1;
	}

	index_t State::GetIndex( const String& name ) const
	{
		auto it = std::find( names_.begin(), names_.end(), name );
		if ( it != names_.end() )
			return it - names_.begin();
		else return NoIndex;
	}

	Real State::GetValue( const String& name ) const
	{
		auto it = std::find( names_.begin(), names_.end(), name );
		SCONE_THROW_IF( it == names_.end(), "Could not find state variable " + name );
		return values_[ it - names_.begin() ];
	}
}
