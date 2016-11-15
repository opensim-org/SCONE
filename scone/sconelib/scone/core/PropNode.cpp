#include "PropNode.h"

#if !USE_FLUT_PROP_NODE

#include <set>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include "boost/filesystem/path.hpp"
#include "Log.h"

using namespace boost;
using namespace boost::property_tree;

namespace scone
{
	const PropNode PropNode::EMPTY = PropNode();

	PropNode::PropNode() : m_Touched( false )
	{
	}

	PropNode::PropNode( const char* prop_string ) : m_Touched( false )
	{
		if ( prop_string == 0 || strlen(prop_string) == 0 )
			return;

		std::string props( prop_string );
		tokenizer< char_separator<char> > tok(props, char_separator<char>(";") );
		for ( tokenizer< char_separator<char> >::iterator iter = tok.begin(); iter != tok.end(); ++iter )
		{
			size_t pos = iter->find_first_of('=');
			if (pos != std::string::npos)
				add_child( trim_copy( iter->substr(0, pos) ) ).m_Value = trim_copy( iter->substr(pos + 1) );
		}
	}

	PropNode::PropNode( const PropNode& other )
	{
		*this = other;
	}

	PropNode::PropNode( PropNode&& other )
	{
		*this = std::move( other );
	}

	PropNode& PropNode::operator=( const PropNode& other )
	{
		if ( this != &other )
		{
			m_Touched = other.m_Touched;
			m_Value = other.m_Value;
			m_Children.clear( );
			for ( const_iterator iter = other.m_Children.begin( ); iter != other.m_Children.end( ); ++iter )
				m_Children.push_back( std::make_pair( iter->first, PropNodePtr( new PropNode( *iter->second ) ) ) );
		}
		return *this;
	}

	PropNode& PropNode::operator=( PropNode&& other )
	{
		m_Touched = std::move( other.m_Touched );
		m_Value = std::move( other.m_Value );
		m_Children = std::move( other.m_Children );
		return *this;
	}

	PropNode& PropNode::Merge( const PropNode& other, bool overwrite )
	{
		if ( overwrite )
			m_Value = other.m_Value;

		iterator insertion_point = begin();

		for ( const_iterator other_it = other.m_Children.begin(); other_it != other.m_Children.end(); ++other_it )
		{
			// see if we already have the key
			PropNode* child = nullptr;
			for ( ChildContainer::iterator this_it = m_Children.begin(); this_it != m_Children.end(); ++this_it )
			{
				if ( this_it->first == other_it->first )
					child = this_it->second.get();
			}

			if ( child == nullptr )
			{
				// add new child at insertion_point
				insertion_point = m_Children.insert( insertion_point, std::make_pair( other_it->first, PropNodePtr( new PropNode( *other_it->second ) ) ) );
				++insertion_point;
			}
			else child->Merge( *other_it->second, overwrite );
		}

		return *this;
	}

	PropNode::~PropNode()
	{
	}

	PropNode& PropNode::add_child( const String& key )
	{
		size_t ofs = key.find_first_of( '.' );
		if ( ofs == String::npos )
		{
			// create first-level child
			m_Children.push_back( std::make_pair( key, PropNodePtr( new PropNode ) ) );
			return *m_Children.back().second;
		}
		else
		{
			log::Warning( "PropNode: using '.' to denote child nodes is deprecated" );
			// create sub-children if necessary
			String head_key = key.substr( 0, ofs );
			String tail_key = key.substr( ofs + 1 );

			PropNode* child = GetChildPtr( head_key );
			if ( child )
				return child->add_child( tail_key );
			else 
				return add_child( head_key ).add_child( tail_key );
		}
	}

	PropNode& PropNode::add_child( const String& key, const PropNode& other )
	{
		return ( add_child( key ) = other );
	}

	PropNode::iterator PropNode::insert_children( const PropNode& other, iterator insertIt )
	{
		for ( const_iterator otherIt = other.begin(); otherIt != other.end(); ++otherIt )
		{
			insertIt = m_Children.insert( insertIt, std::make_pair( otherIt->first, PropNodePtr( new PropNode( *otherIt->second ) ) ) );
			++insertIt;
			//m_Children.back().second->AddChildren( *otherIt->second );
		}

		return insertIt;
	}

	PropNode* PropNode::GetChildPtr( const String& key ) const
	{
		size_t ofs = key.find_first_of( '.' );
		if ( ofs == String::npos )
		{
			// find first-level child
			for ( const_iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter )
			{
				if ( iter->first == key )
					return iter->second.get();
			}
			return nullptr;
		}
		else
		{
			log::Warning( "PropNode: using '.' to denote child nodes is deprecated" );
			// search children
			String head_key = key.substr( 0, ofs );
			String tail_key = key.substr( ofs + 1 );

			for ( const_iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter )
			{
				if ( iter->first == head_key )
				{
					iter->second->touch();
					return iter->second->GetChildPtr( tail_key );
				}
			}
			return nullptr;
		}
	}

	void PropNode::RemoveChild( const String& key )
	{
		size_t ofs = key.find_first_of( '.' );
		if ( ofs == String::npos )
		{
			// remove first matching child
			for ( iterator child_iter = m_Children.begin(); child_iter != m_Children.end(); ++child_iter )
			{
				if ( child_iter->first == key )
				{
					m_Children.erase( child_iter );
					return;
				}
			}
		}
		else
		{
			log::Warning( "PropNode: using '.' to access child nodes is deprecated" );
			// search sub keys
			String head_key = key.substr( 0, ofs );
			String tail_key = key.substr( ofs + 1 );
			PropNode* head_child = GetChildPtr( head_key );
			head_child->RemoveChild( tail_key );
			if ( head_child->empty() )
				RemoveChild( head_key );
		}
	}

	void PropNode::RemoveChildren( const String& key )
	{
		size_t ofs = key.find_first_of( '.' );
		if ( ofs == String::npos )
		{
			// remove all children
			iterator child_iter = m_Children.begin();
			while ( child_iter != m_Children.end() )
			{
				if ( child_iter->first == key )
					child_iter = m_Children.erase( child_iter );
				else ++child_iter;
			}
		}
		else
		{
			log::Warning( "PropNode: using '.' to denote child nodes is deprecated" );
			// search sub keys
			String head_key = key.substr( 0, ofs );
			String tail_key = key.substr( ofs + 1 );
			PropNode* head_child = GetChildPtr( head_key );
			head_child->RemoveChildren( tail_key );
			if ( head_child->empty() )
				RemoveChild( head_key );
		}
	}

	PropNode::iterator PropNode::find_child( const String& key )
	{
		auto lambda = [&]( const KeyChildPair& kcp ) { return kcp.first == key; };
		return std::find_if( m_Children.begin(), m_Children.end(), lambda );
	}

	PropNode::const_iterator PropNode::find_child( const String& key ) const
	{
		auto lambda = [&]( const KeyChildPair& kcp ) { return kcp.first == key; };
		return std::find_if( m_Children.begin(), m_Children.end(), lambda );
	}

	void ToPropertyTree( ptree& tree, const PropNode& props, const String& key )
	{
		ptree& child = key.empty() ? tree : tree.add( key, props.GetValueType() );
		for ( PropNode::const_iterator iter = props.begin(); iter != props.end(); ++iter )
			ToPropertyTree( child, *iter->second, iter->first );
	}

	void FromPropertyTree( PropNode& props, const ptree& tree )
	{
		props.SetValueType( tree.get_value("") );
		for ( const ptree::value_type& v: tree )
		{
			if ( v.first == "<xmlattr>" )
			{
				// directly add XML attributes as children
				FromPropertyTree( props, v.second );
			}
			else if ( v.first != "<xmlcomment>" )
			{
				PropNode& child = props.add_child( v.first );
				FromPropertyTree( child, v.second );
			}
		}
	}

	void PropNode::ToXmlFile( const String& filename, const String& rootname ) const
	{
		ptree pt;
		ToPropertyTree( pt, *this, rootname );
		boost::property_tree::xml_writer_settings< std::string > settings('\t', 1);
		write_xml( filename, pt, std::locale() );
	}

	PropNode& PropNode::FromXmlFile( const String& filename, const String& rootname )
	{
		try
		{
			ptree pt;
			read_xml( filename, pt, boost::property_tree::xml_parser::trim_whitespace );
			FromPropertyTree( *this, rootname.empty() ? pt : pt.get_child( rootname ) );
		}
		catch (file_parser_error& e)
		{
			SCONE_THROW( e.what() );
		}

		return *this;
	}

	void PropNode::ToIniFile( const String& filename ) const
	{
		ptree pt;
		ToPropertyTree( pt, *this, m_Value );
		write_ini( filename, pt );
	}

	PropNode& PropNode::FromIniFile( const String& filename )
	{
		ptree pt;
		read_ini( filename, pt );
		FromPropertyTree( *this, pt );

		return *this;
	}

	void PropNode::ToInfoFile( const String& filename ) const
	{
		ptree pt;
		ToPropertyTree( pt, *this, m_Value );
		write_info( filename, pt );
	}

	PropNode& PropNode::FromInfoFile( const String& filename )
	{
		ptree pt;
		read_info( filename, pt );
		FromPropertyTree( *this, pt );

		return *this;
	}

	std::ostream& PropNode::ToStream( std::ostream& str, const String& prefix, bool unflaggedOnly, int key_width, int depth ) const
	{
		if ( key_width == -1 )
			key_width = GetMaximumKeyWidth( prefix );

		for ( const_iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter )
		{
			if ( !unflaggedOnly || !iter->second->touched() )
			{
				String full_key;
				for ( int i = 0; i < depth; ++i )
					full_key += prefix;
				full_key += iter->first;

				str << std::left << std::setw( key_width ) << full_key;
				if ( iter->second->has_value() )
					str << std::setw( 0 ) << " = " << iter->second->GetValueType();
				str << std::endl;
			}

			iter->second->ToStream( str, prefix, unflaggedOnly, key_width, depth + 1 );
		}

		return str;
	}

	int PropNode::GetMaximumKeyWidth( const String& prefix, int depth ) const
	{
		int kw = 0;
		for ( const_iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter )
		{
			kw = std::max( kw, static_cast< int >( depth * prefix.size() + iter->first.size() ) );
			kw = std::max( kw, iter->second->GetMaximumKeyWidth( prefix, depth + 1 ) );
		}
		return kw;
	}

	size_t PropNode::count_untouched() const
	{
		size_t count = touched() ? 0 : 1;
		for ( const_iterator iter = m_Children.begin(); iter != m_Children.end(); ++iter )
			count += iter->second->count_untouched();
		return count;
	}

	bool PropNode::operator==( const PropNode& other ) const
	{
		if ( m_Value != other.m_Value )
			return false;

		if ( m_Children.size() != other.m_Children.size() )
			return false;

		for ( size_t i = 0; i < m_Children.size(); ++i )
		{
			if ( m_Children[i].first != other.m_Children[i].first )
				return false;
			if ( *m_Children[i].second != *other.m_Children[i].second )
				return false;
		}

		return true;
	}

	void ResolveIncludedFiles( PropNode& props, const String& filename, const String& include_directive, int level )
	{
		SCONE_THROW_IF( level >= 100, "Exceeded maximum include level, check for loops in includes" );

		for ( PropNode::iterator iter = props.begin(); iter != props.end(); )
		{
			if ( iter->first == include_directive )
			{
				// load included file using filename path
				String include_filename = iter->second->get< String >( "file" );
				bool merge_children = iter->second->get< bool >( "merge_children", false );
				boost::filesystem::path include_path = boost::filesystem::path( filename ).parent_path() / include_filename;
				PropNode included_props = ReadPropNode( include_path.string(), include_directive, level + 1 );
				
				// remove the include node
				iter = props.GetChildren().erase( iter );

				// merge or include, depending on options
				if ( merge_children )
				{
					SCONE_ASSERT( included_props.GetChildren().size() == 1 );
					props.Merge( *included_props.begin()->second, false );
					iter = props.begin(); // reset the iterator, which has become invalid after merge
				}
				else
				{
					// insert the children at the INCLUDE spot
					iter = props.insert_children( included_props, iter );
				}
			}
			else
			{
				// search in children
				ResolveIncludedFiles( *iter->second, filename, include_directive, level );
				++iter;
			}
		}

		// untouch all after search
		props.untouch();
	}

	PropNode ReadPropNodeFromXml( const String& filename, const String& include_directive, int level )
	{
		PropNode p;
		p.FromXmlFile( filename );
		ResolveIncludedFiles( p, filename, include_directive, level );
		return p;
	}

	PropNode ReadPropNodeFromInfo( const String& filename, const String& include_directive, int level )
	{
		PropNode p;
		p.FromInfoFile( filename );
		ResolveIncludedFiles( p, filename, include_directive, level );
		return p;
	}

	PropNode ReadPropNodeFromIni( const String& filename, const String& include_directive, int level )
	{
		PropNode p;
		p.FromIniFile( filename );
		ResolveIncludedFiles( p, filename, include_directive, level );
		return p;
	}

	PropNode ReadPropNode( const String& config_file, const String& include_directive, int level )
	{
		String ext = boost::filesystem::path( config_file ).extension().string();
		if ( ext == ".xml" )
			return ReadPropNodeFromXml( config_file, include_directive, level );
		if ( ext == ".info" )
			return ReadPropNodeFromInfo( config_file, include_directive, level );
		if ( ext == ".ini" )
			return ReadPropNodeFromIni( config_file, include_directive, level );
		else SCONE_THROW( "Unknown file type: " + config_file );
	}

	PropNode GetPropNodeFromArgs( int begin_idx, int end_idx, char* argv[] )
	{
		PropNode pn;

		for ( int i = begin_idx; i < end_idx; ++i )
		{
			String s = argv[ i ];
			size_t pos = s.find( "=" );
			if ( pos != String::npos )
			{
				String key = s.substr( 0, pos );
				String value = s.substr( pos + 1 );
				pn.set( key, value );
			}
		}

		return pn;
	}
}

#endif