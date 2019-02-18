/*
** State.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/platform.h"
#include "scone/core/memory_tools.h"
#include <vector>
#include "scone/core/types.h"

namespace scone
{
	class SCONE_API State
	{
	public:
		State() {}
		~State() {}

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
		void SetValue( index_t i, Real value ) { values_.at( i ) = value; }
		void SetValues( const std::vector< Real >& v );

		//void AddChildState( State& other );
		//void CopyChildState( const State& other );

	private:
		//Index parent_idx_;
		std::vector< String > names_;
		std::vector< Real > values_;
	};
}
