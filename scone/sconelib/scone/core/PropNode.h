#pragma once

#include "core.h"
#include "String.h"

#include <memory>
#include <vector>
#include <sstream>

#include "Exception.h"

#if 0

#include "flut/prop_node.hpp"
namespace scone
{
	using PropNode = flut::prop_node;
}

#else

namespace scone
{
	class PropNode;
	typedef std::unique_ptr< PropNode > PropNodePtr;

	class SCONE_API PropNode
	{
	public:
		typedef String KeyType;
		typedef String ValueType;
		typedef std::pair< KeyType, PropNodePtr > KeyChildPair;
		typedef std::vector< KeyChildPair > ChildContainer;
		typedef ChildContainer::iterator iterator;
		typedef ChildContainer::const_iterator const_iterator;

		PropNode();
		PropNode( const PropNode& other );
		PropNode( PropNode&& other );
		explicit PropNode( const char* prop_string );
		~PropNode();

		// assignment
		PropNode& operator=( const PropNode& other );
		PropNode& operator=( PropNode&& other );

		// comparison
		bool operator==( const PropNode& other ) const;
		bool operator!=( const PropNode& other ) const { return !(*this == other); }
		explicit operator bool() const { return !empty();  }

		// check if empty
		bool empty() const { return m_Children.empty() && !HasValue(); }

		// clear
		void clear() { m_Children.clear(); m_Value.clear(); }

		// check if child key exists
		bool has_child( const KeyType& key ) const { return GetChildPtr( key ) != nullptr; }
		bool HasValue() const {	return !m_Value.empty(); }

		// get / set raw value
		const ValueType& GetValueType() const { touch(); return m_Value; }
		void SetValueType( const ValueType& value ) { m_Value = value; }

		// get value
		template< typename T >
		T get( ) const
		{
			touch();
			return GetInternalValue< T >();
		}

		/// Get value, throws exception if key doesn't exist
		template< typename T >
		T get( const KeyType& key ) const
		{
			return get_child( key ).get< T >();
		}

		/// Get value, returns default if key doesn't exist
		template< typename T >
		T get( const KeyType& key, const T& default_value ) const
		{
			touch();
			const PropNode* p = GetChildPtr( key );
			return p ? p->get< T >() : default_value;
		}

		// set value
		template< typename T >
		void set( const T& value )
		{
			SetInternalValue( value );
		}

		/// Set a value, overwriting existing (if any) or adding new key
		template< typename T >
		PropNode& set( const KeyType& key, const T& value )
		{
			PropNode* p = GetChildPtr( key );
			if ( p == nullptr )
				add_child( key ).set( value );
			else p->set( value );
			return *this;
		}

		/// Set a value, always adding new key
		template< typename T >
		PropNode& Add( const KeyType& key, const T& value )
		{
			add_child( key ).set( value );
			return *this;
		}

		/// Get Child
		const PropNode& get_child( const KeyType& key ) const
		{
			const PropNode* p = GetChildPtr( key );
			SCONE_THROW_IF( p == nullptr, "Could not find key: \"" + key + "\"" );
			touch();
			return *p;
		}

		/// Get Child
		PropNode& get_child( const KeyType& key )
		{
			PropNode* p = GetChildPtr( key );
			SCONE_THROW_IF( p == nullptr, "Could not find key: \"" + key + "\"" );
			touch();
			return *p;
		}

		/// Try to get a child, return empty node on failure
		const PropNode& TryGetChild( const KeyType& key ) const
		{
			PropNode* p = GetChildPtr( key );
			if ( !p )
				return EMPTY;
			touch();
			return *p;
		}

		/// create child node
		PropNode& add_child( const KeyType& key );
		PropNode& add_child( const KeyType& key, const PropNode& other );

		/// insert all children from other PropNode
		iterator insert_children( const PropNode& other, iterator insert_point );
		iterator insert_children( const PropNode& other ) { return insert_children( other, begin() ); }

		/// Merge existing properties
		PropNode& Merge( const PropNode& props, bool overwrite = true );

		/// Access to children for iteration
		const ChildContainer& GetChildren() const { touch(); return m_Children; }
		ChildContainer& GetChildren() { touch(); return m_Children; }
		const_iterator begin() const { touch(); return m_Children.cbegin(); }
		const_iterator end() const { touch(); return m_Children.cend(); }
		iterator begin() { touch(); return m_Children.begin(); }
		iterator end() { touch(); return m_Children.end(); }
		iterator find_child( const KeyType& key );
		const_iterator find_child( const KeyType& key ) const;

		/// XML I/O, with optional root name in case there is more than one child
		void ToXmlFile( const String& filename, const KeyType& rootname = "" ) const;
		PropNode& FromXmlFile( const String& filename, const KeyType& rootname = "" );

		void ToIniFile( const String& filename ) const;
		PropNode& FromIniFile( const String& filename );

		void ToInfoFile( const String& filename ) const;
		PropNode& FromInfoFile( const String& filename );

		/// Shortcut 'Get' functions for lazy people
		int GetInt( const KeyType& key ) const { return get< int >( key ); }
		bool GetBool( const KeyType& key ) const { return get< bool >( key ); }
		Real GetReal( const KeyType& key ) const { return get< Real >( key ); }
		String GetStr( const KeyType& key ) const { return get< String >( key ); }

		int GetInt( const KeyType& key, int def ) const { return get< int >( key, def ); }
		bool GetBool( const KeyType& key, bool def ) const { return get< bool >( key, def ); }
		Real GetReal( const KeyType& key, Real def ) const { return get< Real >( key, def ); }
		String GetStr( const KeyType& key, const String& def ) const { return get< String >( key, def ); }

		std::ostream& ToStream( std::ostream& str, const String& prefix = "  ", bool unflaggedOnly = false, int key_width = -1, int depth = 0 ) const;

		// flagging (can be used to detect unused properties)
		const PropNode& touch() const { m_Touched = true; return *this; }
		const PropNode& untouch() const { m_Touched = false; return *this; }
		bool touched() const { return m_Touched; }
		size_t count_untouched() const;

		static const PropNode EMPTY;

	private:
		PropNode* GetChildPtr( const KeyType& key ) const;

		void RemoveChildren( const KeyType& key );
		void RemoveChild( const KeyType& key );

		int GetMaximumKeyWidth( const String& prefix = "  ", int depth = 0 ) const;

		ValueType m_Value;
		ChildContainer m_Children;
		mutable bool m_Touched;

		template< typename T >
		T GetInternalValue() const {
			std::stringstream ss( m_Value );
			T value;
			ss >> value;
			return value;
		}

		template< typename T >
		void SetInternalValue( T& value ) {
			std::ostringstream ss;
			ss << value;
			m_Value = ss.str();
		}
	};

 	template<> inline
    String PropNode::GetInternalValue< String >() const {
        return m_Value;
    }


	// shortcut file readers for lazy people
	PropNode SCONE_API ReadPropNodeFromXml( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode SCONE_API ReadPropNodeFromInfo( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode SCONE_API ReadPropNodeFromIni( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode SCONE_API ReadPropNode( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode SCONE_API GetPropNodeFromArgs( int begin_idx, int end_idx, char* argv[] );

	// stream operator
	inline std::ostream& operator<<( std::ostream& str, const PropNode& props ) { props.ToStream( str ); return str; }
};

#endif
