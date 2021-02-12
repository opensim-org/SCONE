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
	class SCONE_API State
	{
	public:
		State() = default;

		size_t GetSize() const { return names_.size(); }

		const std::vector< String >& GetNames() const { return names_; }

		const Real& GetValue( index_t i ) const { return values_[ i ]; }
		const Real& operator[]( index_t i ) const { return values_[ i ]; }
		Real& operator[]( index_t i ) { return values_[ i ]; }

		const std::vector<Real>& GetValues() const { return values_; }
		std::vector<Real>& GetValues() { return values_; }

		index_t FindIndex( const String& name ) const;
		index_t FindIndexByPattern( const String& pattern, index_t start_index = 0 ) const;
		const String& GetName( index_t i ) const { return names_[ i ]; }

		index_t AddVariable( const String& name, Real value = Real( 0 ) );
		void SetValue( index_t i, Real value ) { SCONE_ASSERT( i < values_.size() ); values_[ i ] = value; }
		void SetValues( const std::vector<Real>& v );

	private:
		std::vector<String> names_;
		std::vector<Real> values_;
	};
}
