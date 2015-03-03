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
	typedef std::unique_ptr< PropNode > PropNodePtr;

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
		bool IsEmpty() { return m_Children.empty() && !HasValue(); }

		// clear
		void Clear() { m_Children.clear(); m_Value.clear(); }

		// check if child key exists
		bool HasKey( const String& key ) const { return GetChildPtr( key ) != nullptr; }

		// get value
		bool HasValue() const {	return !m_Value.empty(); }

		// get value
		const String& GetValue() const { return m_Value; }

		// set value
		void SetValue( const String& value ) { m_Value = value; }

		// get and cast value of fundamental or Vec3 types
		template< typename T >
		T Get( typename std::enable_if< std::is_fundamental< T >::value || std::is_same< T, Vec3 >::value >::type* = 0 ) const
		{
			T value;
			std::istringstream str( m_Value );
			str >> value;
			return value;
		}

		// get string value
		template< typename T >
		T Get( typename std::enable_if< std::is_same< T, String >::value >::type* = 0 ) const
		{
			return m_Value;
		}

		/// Get value, throws exception if key doesn't exist
		template< typename T >
		T Get( const String& key ) const
		{
			return GetChild( key ).Get< T >();
		}

		/// Get value, returns default if key doesn't exist
		template< typename T >
		T Get( const String& key, const T& default_value ) const
		{
			const PropNode* p = GetChildPtr( key );
			return p ? p->Get< T >() : default_value;
		}

		// set value
		template< typename T >
		void Set( const T& value )
		{
			std::ostringstream str;
			str << value;
			m_Value = str.str();
		}

		/// Set a value, overwriting existing (if any) or adding new key
		template< typename T >
		PropNode& Set( const String& key, const T& value )
		{
			PropNode* p = GetChildPtr( key );
			if ( p == nullptr )
				AddChild( key ).Set( value );
			else p->Set( value );
			return *this;
		}

		/// Set a value, always adding new key
		template< typename T >
		PropNode& Add( const String& key, const T& value )
		{
			AddChild( key ).Set( value );
			return *this;
		}

		/// Get Child
		const PropNode& GetChild( const String& key ) const
		{
			const PropNode* p = GetChildPtr( key );
			if ( p == nullptr )
				SCONE_THROW( "Could not find key: " + key );
			else return *p;
		}

		/// create child node
		PropNode& AddChild( const String& key );

		/// Merge existing properties
		PropNode& Merge( const PropNode& props, bool overwrite = true );

		/// Access to children for iteration
		const ChildContainer& GetChildren() const { return m_Children; }
		ConstChildIter Begin() const { return m_Children.cbegin(); }
		ConstChildIter End() const { return m_Children.cend(); }

		/// XML I/O, with optional root name in case there is more than one child
		void ToXmlFile( const String& filename, const String& rootname = "" );
		PropNode& FromXmlFile( const String& filename, const String& rootname = "" );

		void ToIniFile( const String& filename );
		PropNode& FromIniFile( const String& filename );

		void ToInfoFile( const String& filename );
		PropNode& FromInfoFile( const String& filename );

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

		void ToStream( std::ostream& str, const std::string& prefix = "", bool unflaggedOnly = false ) const;

		// flagging (can be used to detect unused properties)
		void SetFlag() const { m_Flag = true; }
		void ClearFlag() const { m_Flag = false; }
		bool GetFlag() const { return m_Flag; }

	private:
		PropNode* GetChildPtr( const String& key ) const;

		void RemoveChildren( const String& key );
		void RemoveChild( const String& key );

		String m_Value;
		ChildContainer m_Children;
		mutable bool m_Flag;
	};

	// shortcut file readers for lazy people
	PropNode CORE_API CreatePropNodeFromXmlFile( const String& filename );
	PropNode CORE_API CreatePropNodeFromInfoFile( const String& filename );

	// stream operator
	inline std::ostream& operator<<( std::ostream& str, const PropNode& props ) { props.ToStream( str ); return str; }
};
