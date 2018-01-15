#pragma once

#include "scone/core/core.h"
#include "scone/core/memory_tools.h"
#include <vector>

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

		Real GetValue( Index i ) const { return values_[ i ]; }
		Real GetValue( const String& i ) const;

		const Real& operator[]( Index i ) const { return values_[ i ]; }
		Real& operator[]( Index i ) { return values_[ i ]; }

		Index GetIndex( const String& name ) const;
		const String& GetName( Index i ) const { return names_[ i ]; }

		Index AddVariable( const String& name, Real value = Real( 0 ) );
		void SetValue( Index i, Real value ) { values_.at( i ) = value; }
		void SetValues( const std::vector< Real >& v );

		//void AddChildState( State& other );
		//void CopyChildState( const State& other );

	private:
		//Index parent_idx_;
		std::vector< String > names_;
		std::vector< Real > values_;
	};
}
