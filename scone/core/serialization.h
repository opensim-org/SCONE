#pragma once

#include "PropNode.h"
#include <map>
#include <vector>
#include <list>
#include "has_member.hpp"

namespace scone
{
	// i/o helper defines
	#define SCONE_ENABLE_SERIALIZATION( _classname_ ) template <> struct has_process_data< _classname_ > : public std::true_type {};
	#define SCONE_PROCESS_DATA_MEMBER( _var_ ) ::scone::ProcessData( _archive_, _read_, #_var_, _var_ );
	#define SCONE_PROCESS_DATA_MEMBER_NAMED( _var_, _name_ ) ::scone::ProcessData( _archive_, _read_, _name_, _var_ );
	#define SCONE_PROCESS_DATA_MEMBERS template< typename Archive > void ProcessData( Archive& _archive_, bool _read_ )

	// is_container
	template<class T> struct is_container : public std::false_type {};
	template<class T, class Alloc> 
	struct is_container<std::vector<T, Alloc>> : public std::true_type {};
	template<class T, class Alloc> 
	struct is_container<std::list<T, Alloc>> : public std::true_type {};

	// testing
	template<class T> struct has_process_data : public std::false_type {};

	// has_member test
	struct check_has_test
	{
		template <typename T, void (T::*)() const = &T::test>
		struct get
		{ };
	};

	template <typename T>
	struct has_test :
		has_member<T, check_has_test>
	{ };

	// top level PropNode write
	template< typename T >
	void WriteData( PropNode& props, T& v ) {
		v.ProcessData( props, false );
	}

	// top level PropNode read
	template< typename T >
	void ReadData( PropNode& props, T& v ) {
		v.ProcessData( props, true );
	}

	// PropNode vector process
	template< typename T >
	void ProcessData( PropNode& props, bool read, const String& name, std::vector< T >& v )
	{
		if ( read )
		{
			v.clear();
			PropNodePtr vec_node = props.GetChildPtr( name );
			for ( PropNode::ConstChildIter iter = vec_node->Begin(); iter != vec_node->End(); ++iter )
			{
				v.push_back( T() );
				ProcessData( *vec_node, true, iter->first, v.back() );
			}
		}
		else
		{
			int element = 0;
			PropNodePtr vec_node = props.AddChild( name );
			for ( std::vector< T >::iterator iter = v.begin(); iter != v.end(); ++iter )
			{
				ProcessData( *vec_node, false, GetStringF( "item%d", element++ ), *iter );
			}
		}
	}

	// PropNode for types that have a ProcessData function
	template< typename T >
	void ProcessData( PropNode& props, bool read, const String& name, T& v, typename std::enable_if< has_process_data< T >::value >::type* = 0 )
	{
		if ( read )
		{
			PropNodePtr p = props.GetChildPtr( name );
			v.ProcessData( *p, true );
		}
		else
		{
			PropNodePtr p = props.AddChild( name );
			v.ProcessData( *p, false );
		}
	}

	// PropNode for other types
	template< typename T >
	void ProcessData( PropNode& props, bool read, const String& name, T& v, typename std::enable_if< !has_process_data< T >::value >::type* = 0 )
	{
		if ( read )
			v = props.Get<T>( name );
		else
			props.Add( name, v );
	}
}
