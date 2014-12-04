#pragma once

#include "core.h"

#include <memory>
#include <vector>
#include <sstream>
#include <type_traits>

#include "Vec3.h"
#include "Exception.h"
#include "tools.h"

namespace scone
{
	class PropNode;
	typedef std::shared_ptr< PropNode > PropNodePtr;

	class StringValue
	{
	public:
		StringValue() { };
		StringValue( const char* value ) : m_Data( value ) { };
		StringValue( const StringValue& other ) : m_Data( other.m_Data ) { };
		StringValue& operator=( const StringValue& other ) { m_Data = other.m_Data; return (*this); }
		bool operator==( const StringValue& other ) const { return m_Data == other.m_Data; }
		bool operator!=( const StringValue& other ) const { return !(*this == other); }
		template< typename T > StringValue( const T& value ) { *this = value; }
		~StringValue() { };

		// get value of any type T
		template< typename T >
		operator T() const
		{
			std::istringstream str( m_Data );
			T value;
			str >> value;
			return value;
		}

		// string is a special case
		operator String() const
		{
			return m_Data;
		}

		// set values to any type T
		template< typename T >
		StringValue& operator=( const T& value )
		{
			std::ostringstream str;
			str << value;
			m_Data = str.str();
			return (*this);
		}

		const String& GetData() const { return m_Data; }
		bool IsEmpty() const { return m_Data.empty(); }

	private:
		String m_Data;
	};

	class CORE_API PropNode
	{
	public:

		typedef std::pair< String, PropNodePtr > KeyChildPair;
		typedef std::vector< KeyChildPair > ChildContainer;
		typedef ChildContainer::iterator ChildIter;
		typedef ChildContainer::const_iterator ConstChildIter;

		PropNode();
		PropNode( const PropNode& other );
		PropNode( const char* prop_string );
		~PropNode();

		// assignment
		PropNode& operator=( const PropNode& other );

		// comparison
		bool operator==( const PropNode& other ) const;
		bool operator!=( const PropNode& other ) const { return !(*this == other); }

		// check if empty
		bool IsEmpty()
		{
			return m_Children.empty() && m_Value.IsEmpty();
		}

		// check if empty
		void Clear()
		{
			m_Children.clear();
			m_Value = "";
		}

		// check if child key exists
		bool HasKey( const String& key ) const
		{
			return GetChildPtr( key ) != nullptr;
		}

		// get value
		const StringValue& GetValue() const
		{
			return m_Value;
		}

		// get value
		bool HasValue() const
		{
			return !m_Value.IsEmpty();
		}

		// get and cast value
		template< typename T >
		T Get() const
		{
			return static_cast< T >( m_Value );
		}

		// set value
		template< typename T >
		void Set( const T& value )
		{
			m_Value = value;
		}

		/// Get value, throws exception if key doesn't exist
		template< typename T >
		T Get( const String& key ) const
		{
			PropNodePtr p = GetChildPtr( key );
			if ( p == nullptr )
				SCONE_THROW( "Could not find key: " + key );
			else return p->Get<T>();
		}

		/// Get value, returns default if key doesn't exist
		template< typename T >
		T Get( const String& key, const T& default_value ) const
		{
			PropNodePtr p = GetChildPtr( key );
			return p ? p->Get<T>() : default_value;
		}

		/// Copy child properties, empty if not existing
		PropNode CopyChild( const String& key ) const
		{
			PropNodePtr p = GetChildPtr( key );
			if ( p == nullptr )
				return PropNode();
			else return PropNode( *p );
		}

		/// Set a value, overwriting existing (if any) or adding new key
		template< typename T >
		PropNode& Set( const String& key, const T& value )
		{
			PropNodePtr p = GetChildPtr( key );
			if ( p == nullptr )
				p = AddChild( key );
			p->Set( value );
			return *this;
		}

		/// Set a value, always adding new key
		template< typename T >
		PropNode& Add( const String& key, const T& value )
		{
			AddChild( key )->Set( value );
			return *this;
		}

		/// Get Child
		const PropNodePtr GetChildPtr( const String& key ) const;

		/// create child node
		PropNodePtr AddChild( const String& key );

		/// Merge existing properties
		PropNode& Merge( const PropNode& props, bool overwrite = true );

		/// Access to children for iteration
		const ChildContainer& GetChildren() const { return m_Children; }
		ConstChildIter Begin() const { return m_Children.cbegin(); }
		ConstChildIter End() const { return m_Children.cend(); }

		/// XML I/O, with optional root name in case there is more than one child
		void ToXmlFile( const String& filename, const String& rootname = "" );
		void FromXmlFile( const String& filename, const String& rootname = "" );

		void ToIniFile( const String& filename );
		void FromIniFile( const String& filename );

		void ToInfoFile( const String& filename );
		void FromInfoFile( const String& filename );

		/// Shortcut 'Get' functions for lazy people
		int GetInt( const String& key ) const { return Get< int >( key ); }
		bool GetBool( const String& key ) const { return Get< bool >( key ); }
		Real GetReal( const String& key ) const { return Get< Real >( key ); }
		String GetStr( const String& key ) const { return Get< String >( key ); }
		Vec3 GetVec3( const String& key ) const { return Get< Vec3 >( key ); }

		int GetInt( const String& key, int def ) const { return Get< int >( key, def ); }
		bool GetBool( const String& key, bool def ) const { return Get< bool >( key, def ); }
		Real GetReal( const String& key, Real def ) const { return Get< Real >( key, def ); }
		String GetStr( const String& key, const String& def ) const { return Get< String >( key, def ); }
		Vec3 GetVec3( const String& key, const Vec3& def ) const { return Get< Vec3 >( key, def ); }

		friend std::ostream& operator<<( std::ostream& str, const PropNode& props ) { props.ToStream( str ); return str; }

	private:

		void ToStream( std::ostream& str, const std::string& prefix = "" ) const;
		void RemoveChildren( const String& key );
		void RemoveChild( const String& key );

		StringValue m_Value;
		ChildContainer m_Children;
	};
};
