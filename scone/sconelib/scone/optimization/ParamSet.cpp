#include "Params.h"
#include "scone/core/Exception.h"
#include <algorithm>
#include <fstream>
#include <boost/format.hpp>
#include "scone/core/Log.h"

#include "scone/core/string_tools.h"

namespace scone
{
	ParamInterface::ParamInterface( const path& filename ) : m_Mode( ConstructionMode )
	{
		Read( filename, true );
	}

	double ParamInterface::Get( const ParamInfo& info )
	{
		if ( m_Mode == ConstructionMode )
		{
			// check if a parameter with the same name already exists
			auto iter = FindParamByName( info.name );
			if ( iter != m_Params.end() )
			{
				//SCONE_LOG( "Duplicate parameter during construction: " << info.name );
				return iter->second;
			}

			// add parameter info
			// TODO: perhaps skip parameters that are not free
			m_Params.push_back( std::make_pair( info, info.GetInitialValue() ) );

			return m_Params.back().second;
		}
		else if ( m_Mode == UpdateMode )
		{
			auto iter = FindParamByName( info.name );
			if ( iter == m_Params.end() )
				SCONE_THROW( "Could not find parameter: " + info.name );

			// return parameter value
			return iter->second;
		}
		SCONE_THROW( "Invalid mode: " + to_str( m_Mode ) );
	}

	double ParamInterface::Get( const String& name, const PropNode& props, const String& node_name )
	{
		// get par node (throws if not exists)
		const PropNode& parNode = props.get_child( node_name );

		// see if this is an actual parameter
		if ( parNode.size() > 0 )
			return Get( ParamInfo( GetNamePrefix() + name, parNode ) );
		else if ( parNode.has_value() )
			return parNode.get< double >(); // just return the value

		SCONE_THROW( "Could not read parameter " + node_name );
	}

	double ParamInterface::Get( const String& name, const PropNode& props, const String& node_name, double default_value )
	{
		if ( props.has_key( node_name ) )
			return Get( name, props, node_name );
		else return default_value;
	}

	double ParamInterface::GetMeanStd( const String& name, double init_mean, double init_std, double min, double max )
	{
		return Get( ParamInfo( GetNamePrefix() + name, init_mean, init_std, 0.0, 0.0, min, max ) );
	}

	double ParamInterface::GetMinMax( const String& name, double init_min, double init_max, double min, double max )
	{
		return Get( ParamInfo( GetNamePrefix() + name, 0.0, 0.0, init_min, init_max, min, max ) );
	}

	std::vector< std::pair< ParamInfo, double > >::iterator ParamInterface::FindParamByName( const String& name )
	{
		return std::find_if( m_Params.begin(), m_Params.end(), [&]( const std::pair< ParamInfo, double >& v ) { return v.first.name == name; } );
	}

	size_t ParamInterface::GetFreeParamCount()
	{
		return std::count_if( m_Params.begin(), m_Params.end(), [&]( const std::pair< ParamInfo, double >& v ) { return v.first.is_free; } );
	}

	std::vector< double > ParamInterface::GetFreeParamValues()
	{
		std::vector< double > vec;
		for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
		{
			if ( iter->first.is_free )
				vec.push_back( iter->second );
		}
		return vec;
	}

	void ParamInterface::SetFreeParamValues( const std::vector< double >& values )
	{
		auto vecIter = values.begin();
		for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
		{
			if ( iter->first.is_free )
			{
				SCONE_ASSERT( vecIter != values.end() );
				iter->second = *( vecIter++ );
			}
		}
	}

	bool ParamInterface::CheckValues()
	{
		return std::find_if( m_Params.begin(), m_Params.end(),
			[&]( const std::pair< ParamInfo, double >& v ) { return !v.first.CheckValue( v.second ); } ) == m_Params.end();
	}

	void ParamInterface::ClampValues()
	{
		std::for_each( m_Params.begin(), m_Params.end(),
			[&]( std::pair< ParamInfo, double >& v ) { v.first.ClampValue( v.second ); } );
	}

	void ParamInterface::InitRandom()
	{
		for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
		{
			if ( iter->first.is_free )
				iter->second = iter->first.GetInitialValue();
		}
	}

	void ParamInterface::Write( const path& filename ) const
	{
		std::ofstream ofstr( filename.str() );
		SCONE_THROW_IF( !ofstr.good(), "Error opening file: " + filename.str() );
		ToStream( ofstr );
	}

	void ParamInterface::Read( const path& filename, bool read_std )
	{
		log::debug( "Reading " + quoted( filename.str() ) );
		std::ifstream ifstr( filename.str() );
		SCONE_THROW_IF( !ifstr.good(), "Error opening file: " + filename.str() );
		FromStream( ifstr, read_std );
	}

	void ParamInterface::UpdateMeanStd( const std::vector< ParamInterface >& parsets )
	{
		for ( size_t parIdx = 0; parIdx < m_Params.size(); ++parIdx )
		{
			double w = 1.0 / parsets.size();

			double mean = 0.0;
			for ( size_t setIdx = 0; setIdx < parsets.size(); ++setIdx )
				mean += w * parsets[ setIdx ].m_Params[ parIdx ].second;

			double var = 0.0;
			for ( size_t setIdx = 0; setIdx < parsets.size(); ++setIdx )
				var += w * GetSquared( parsets[ setIdx ].m_Params[ parIdx ].second - mean );

			// update the result in ParInfo
			// TODO: keep this somewhere else?
			m_Params[ parIdx ].first.init_mean = mean;
			m_Params[ parIdx ].first.init_std = sqrt( var );
		}
	}

	void ParamInterface::UpdateMeanStd( const std::vector< double >& means, const std::vector< double >& stds )
	{
		SCONE_ASSERT( means.size() == stds.size() );
		size_t vec_idx = 0;
		for ( size_t params_idx = 0; params_idx < m_Params.size(); ++params_idx )
		{
			if ( m_Params[ params_idx ].first.is_free )
			{
				SCONE_ASSERT( vec_idx < means.size() );
				m_Params[ params_idx ].first.init_mean = means[ vec_idx ];
				m_Params[ params_idx ].first.init_std = stds[ vec_idx ];
				++vec_idx;
			}
		}
	}

	void ParamInterface::PushNamePrefix( const String& prefix )
	{
		m_NamePrefixes.push_back( prefix );
	}

	void ParamInterface::PopNamePrefix()
	{
		SCONE_ASSERT( m_NamePrefixes.size() > 0 );

		m_NamePrefixes.pop_back();
	}

	String ParamInterface::GetNamePrefix() const
	{
		String full_prefix;
		for ( const String& s : m_NamePrefixes )
			full_prefix += s;
		return full_prefix;
	}

	void ParamInterface::SetGlobalStd( double factor, double offset )
	{
		for ( auto& p : m_Params )
			p.first.init_std = factor * fabs( p.first.init_mean ) + offset;
	}

	std::ostream& ParamInterface::ToStream( std::ostream& str ) const
	{
		for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
		{
			if ( iter->first.is_free )
				str << boost::format( "%-35s\t%14.8f\t%14.8f\t%14.8f\n" ) % iter->first.name % iter->second % iter->first.init_mean % iter->first.init_std;
		}

		return str;
	}

	std::istream& ParamInterface::FromStream( std::istream& str, bool load_std )
	{
		size_t params_set = 0;
		size_t params_not_found = 0;

		while ( str.good() )
		{
			std::string name;
			double value, mean, std;
			str >> name >> value >> mean >> std;

			if ( name.empty() )
				continue;

			std::vector< std::pair< ParamInfo, double > >::iterator iter = FindParamByName( name );
			if ( iter != m_Params.end() )
			{
				// read existing parameter, updating mean / std
				iter->second = value;
				iter->first.init_mean = mean;
				if ( load_std )
					iter->first.init_std = std;
				++params_set;
			}
			else
			{
				// create new parameter if in construction mode
				if ( IsInConstructionMode() )
				{
					// TODO: use a single mechanism to create parameters
					ParamInfo info = ParamInfo( name, mean, std, 0, 0, REAL_LOWEST, REAL_MAX );
					m_Params.push_back( std::make_pair( info, value ) );
				}

				++params_not_found;
			}
		}

		log::debug( "Parameters read=", params_set, " new parameters=", params_not_found );

		return str;
	}
}
