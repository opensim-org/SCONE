#include "State.h"

#include <algorithm>
#include "scone/core/Exception.h"

namespace scone
{
	Index State::AddVariable( const String& name, Real value )
	{
		names_.push_back( name );
		values_.push_back( value );
		return names_.size() - 1;
	}

	void State::SetValues( const std::vector< Real >& v )
	{
		SCONE_ASSERT( values_.size() == v.size() );
		copy( v.begin(), v.end(), values_.begin() );
	}

	void State::AddChildState( State& other )
	{
		other.parent_idx_ = names_.size();
		names_.insert( names_.end(), other.GetNames().begin(), other.GetNames().end() );
		values_.insert( values_.end(), other.GetValues().begin(), other.GetValues().end() );
	}

	void State::CopyChildState( const State& other )
	{
		SCONE_ASSERT( other.GetSize() <= GetSize() - other.parent_idx_ );
		std::copy( other.GetValues().begin(), other.GetValues().end(), values_.begin() + other.parent_idx_ );
	}

	Index State::GetIndex( const String& name ) const
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
