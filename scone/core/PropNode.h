#pragma once

#include "core.h"

#include <memory>
#include <vector>
#include <sstream>
#include <type_traits>

#include "Vec3.h"
#include "Exception.h"

namespace scone
{
	class PropNode;
	typedef std::unique_ptr< PropNode > PropNodePtr;

	class CORE_API PropNode
	{
	public:
		typedef String KeyType;
		typedef String ValueType;
		typedef std::pair< KeyType, PropNodePtr > KeyChildPair;
		typedef std::vector< KeyChildPair > ChildContainer;
		typedef ChildContainer::iterator ChildIter;
		typedef ChildContainer::const_iterator ConstChildIter;

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
		explicit operator bool() const { return !IsEmpty();  }

		// check if empty
		bool IsEmpty() const { return m_Children.empty() && !HasValue(); }

		// clear
		void Clear() { m_Children.clear(); m_Value.clear(); }

		// check if child key exists
		bool HasKey( const KeyType& key ) const { return GetChildPtr( key ) != nullptr; }
		bool HasValue() const {	return !m_Value.empty(); }

		// get / set raw value
		const ValueType& GetValueType() const { Touch(); return m_Value; }
		void SetValueType( const ValueType& value ) { m_Value = value; }

		// get value
		template< typename T >
		T GetValue( ) const
		{
			Touch();
			return GetInternalValue< T >();
		}

		/// Get value, throws exception if key doesn't exist
		template< typename T >
		T Get( const KeyType& key ) const
		{
			return GetChild( key ).GetValue< T >();
		}

		/// Get value, returns default if key doesn't exist
		template< typename T >
		T Get( const KeyType& key, const T& default_value ) const
		{
			Touch();
			const PropNode* p = GetChildPtr( key );
			return p ? p->GetValue< T >() : default_value;
		}

		// set value
		template< typename T >
		void Set( const T& value )
		{
			SetInternalValue( value );
		}

		/// Set a value, overwriting existing (if any) or adding new key
		template< typename T >
		PropNode& Set( const KeyType& key, const T& value )
		{
			PropNode* p = GetChildPtr( key );
			if ( p == nullptr )
				AddChild( key ).Set( value );
			else p->Set( value );
			return *this;
		}

		/// Set a value, always adding new key
		template< typename T >
		PropNode& Add( const KeyType& key, const T& value )
		{
			AddChild( key ).Set( value );
			return *this;
		}

		/// Get Child
		const PropNode& GetChild( const KeyType& key ) const
		{
			const PropNode* p = GetChildPtr( key );
			SCONE_THROW_IF( p == nullptr, "Could not find key: \"" + key + "\"" );
			Touch();
			return *p;
		}

		/// Get Child
		PropNode& GetChild( const KeyType& key )
		{
			PropNode* p = GetChildPtr( key );
			SCONE_THROW_IF( p == nullptr, "Could not find key: \"" + key + "\"" );
			Touch();
			return *p;
		}

		/// Try to get a child, return empty node on failure
		const PropNode& TryGetChild( const KeyType& key ) const
		{
			PropNode* p = GetChildPtr( key );
			if ( !p )
				return EMPTY;
			Touch();
			return *p;
		}

		/// create child node
		PropNode& AddChild( const KeyType& key );
		PropNode& AddChild( const KeyType& key, const PropNode& other );

		/// insert all children from other PropNode
		ChildIter InsertChildren( const PropNode& other, ChildIter insert_point );
		ChildIter InsertChildren( const PropNode& other ) { return InsertChildren( other, Begin() ); }

		/// Merge existing properties
		PropNode& Merge( const PropNode& props, bool overwrite = true );

		/// Access to children for iteration
		const ChildContainer& GetChildren() const { Touch(); return m_Children; }
		ChildContainer& GetChildren() { Touch(); return m_Children; }
		ConstChildIter Begin() const { Touch(); return m_Children.cbegin(); }
		ConstChildIter End() const { Touch(); return m_Children.cend(); }
		ChildIter Begin() { Touch(); return m_Children.begin(); }
		ChildIter End() { Touch(); return m_Children.end(); }
		ChildIter FindChild( const KeyType& key );
		ConstChildIter FindChild( const KeyType& key ) const;

		/// XML I/O, with optional root name in case there is more than one child
		void ToXmlFile( const String& filename, const KeyType& rootname = "" );
		PropNode& FromXmlFile( const String& filename, const KeyType& rootname = "" );

		void ToIniFile( const String& filename );
		PropNode& FromIniFile( const String& filename );

		void ToInfoFile( const String& filename );
		PropNode& FromInfoFile( const String& filename );

		/// Shortcut 'Get' functions for lazy people
		int GetInt( const KeyType& key ) const { return Get< int >( key ); }
		bool GetBool( const KeyType& key ) const { return Get< bool >( key ); }
		Real GetReal( const KeyType& key ) const { return Get< Real >( key ); }
		String GetStr( const KeyType& key ) const { return Get< String >( key ); }
		Vec3 GetVec3( const KeyType& key ) const { return Get< Vec3 >( key ); }

		int GetInt( const KeyType& key, int def ) const { return Get< int >( key, def ); }
		bool GetBool( const KeyType& key, bool def ) const { return Get< bool >( key, def ); }
		Real GetReal( const KeyType& key, Real def ) const { return Get< Real >( key, def ); }
		String GetStr( const KeyType& key, const String& def ) const { return Get< String >( key, def ); }
		Vec3 GetVec3( const KeyType& key, const Vec3& def ) const { return Get< Vec3 >( key, def ); }

		std::ostream& ToStream( std::ostream& str, const String& prefix = "  ", bool unflaggedOnly = false, int key_width = -1, int depth = 0 ) const;

		// flagging (can be used to detect unused properties)
		const PropNode& Touch() const { m_Touched = true; return *this; }
		const PropNode& UnTouch() const { m_Touched = false; return *this; }
		bool IsTouched() const { return m_Touched; }
		size_t GetUntouchedCount() const;

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

		template<>
		String GetInternalValue< String >() const {
			return m_Value;
		}

		template< typename T >
		void SetInternalValue( T& value ) {
			std::ostringstream ss;
			ss << value;
			m_Value = ss.str();
		}
	};

	// shortcut file readers for lazy people
	PropNode CORE_API ReadPropNodeFromXml( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode CORE_API ReadPropNodeFromInfo( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode CORE_API ReadPropNodeFromIni( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode CORE_API ReadPropNode( const String& filename, const PropNode::KeyType& include_directive = "INCLUDE", int level = 0 );
	PropNode CORE_API GetPropNodeFromArgs( int begin_idx, int end_idx, char* argv[] );

	// stream operator
	inline std::ostream& operator<<( std::ostream& str, const PropNode& props ) { props.ToStream( str ); return str; }
};
