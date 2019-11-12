/*
** State.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "scone/core/Exception.h"
#include "scone/core/types.h"
#include "xo/container/container_tools.h"
#include <vector>

namespace scone
{
	// #todo: consider making this a simple(r) struct
	class SCONE_API State
	{
	public:
		State() = default;

		size_t GetSize() const { return names_.size(); }
		const std::vector< String >& GetNames() const { return names_; }
		const std::vector< Real >& GetValues() const { return values_; }

		Real GetValue( index_t i ) const { return values_[ i ]; }
		Real GetValue( const String& i ) const;

		const Real& operator[]( index_t i ) const { return values_[ i ]; }
		Real& operator[]( index_t i ) { return values_[ i ]; }

		index_t GetIndex( const String& name ) const;
		const String& GetName( index_t i ) const { return names_[ i ]; }

		index_t AddVariable( const String& name, Real value = Real( 0 ) );
		void SetValue( index_t i, Real value ) { SCONE_ASSERT( i < values_.size() ); values_[ i ] = value; }

		template< typename T >
		void SetValues( const std::vector<T>& v ) {
			SCONE_ASSERT( values_.size() <= v.size() );
			xo::copy_cast( v.begin(), v.begin() + values_.size(), values_.begin() );
		}

	private:
		std::vector< String > names_;
		std::vector< Real > values_;
	};
}
