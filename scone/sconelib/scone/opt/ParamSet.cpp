#include "ParamSet.h"
#include "scone/core/Exception.h"
#include <algorithm>
#include <fstream>
#include <boost/format.hpp>
#include "scone/core/Log.h"

#include "scone/core/string_tools.h"

namespace scone
{
	namespace opt
	{
		ParamSet::ParamSet( const String& filename ) : m_Mode( ConstructionMode )
		{
			Read( filename );
		}

		double ParamSet::Get( const ParamInfo& info )
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

		double ParamSet::Get( const String& name, const PropNode& props, const String node_name )
		{
			// get par node (throws if not exists)
			const PropNode& parNode = props.GetChild( node_name );

			// see if this is an actual parameter
			if ( parNode.GetChildren().size() > 0 )
				return Get( ParamInfo( GetNamePrefix() + name, parNode ) );
			else return props.GetValue< double >(); // just return the value
		}

		double ParamSet::TryGet( const String& name, const PropNode& props, const String node_name, double default_value )
		{
			if ( props.HasKey( node_name ) )
				return Get( name, props, node_name );
			else return default_value;
		}

		double ParamSet::GetMeanStd( const String& name, double init_mean, double init_std, double min, double max )
		{
			return Get( ParamInfo( GetNamePrefix() + name, init_mean, init_std, 0.0, 0.0, min, max ) );
		}

		double ParamSet::GetMinMax( const String& name, double init_min, double init_max, double min, double max )
		{
			return Get( ParamInfo( GetNamePrefix() + name, 0.0, 0.0, init_min, init_max, min, max ) );
		}

		std::vector< std::pair< ParamInfo, double > >::iterator ParamSet::FindParamByName( const String& name )
		{
			return std::find_if( m_Params.begin(), m_Params.end(), [&]( const std::pair< ParamInfo, double >& v ) { return v.first.name == name; } );
		}

		size_t ParamSet::GetFreeParamCount()
		{
			return std::count_if( m_Params.begin(), m_Params.end(), [&]( const std::pair< ParamInfo, double >& v ) { return v.first.is_free; } );
		}

		std::vector< double > ParamSet::GetFreeParamValues()
		{
			std::vector< double > vec;
			for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
			{
				if ( iter->first.is_free )
					vec.push_back( iter->second );
			}
			return vec;
		}

		void ParamSet::SetFreeParamValues( std::vector< double >& values )
		{
			auto vecIter = values.begin();
			for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
			{
				if ( iter->first.is_free )
				{
					SCONE_ASSERT( vecIter != values.end() );
					iter->second = *(vecIter++);
				}
			}
		}

		bool ParamSet::CheckValues()
		{
			return std::find_if( m_Params.begin(), m_Params.end(),
				[&]( const std::pair< ParamInfo, double >& v ) { return !v.first.CheckValue( v.second ); } ) == m_Params.end();
		}

		void ParamSet::ClampValues()
		{
			std::for_each( m_Params.begin(), m_Params.end(),
				[&]( std::pair< ParamInfo, double >& v ) { v.first.ClampValue( v.second ); } );
		}

		void ParamSet::InitRandom()
		{
			for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
			{
				if ( iter->first.is_free )
					iter->second = iter->first.GetInitialValue();
			}
		}

		void ParamSet::Write( const String& filename ) const
		{
			std::ofstream ofstr( filename );
			SCONE_THROW_IF( !ofstr.good(), "Error opening file: " + filename );
			ToStream( ofstr );
		}

		void ParamSet::Read( const String& filename )
		{
			log::Debug( "Reading " + quoted( filename ) );
			std::ifstream ifstr( filename );
			SCONE_THROW_IF( !ifstr.good(), "Error opening file: " + filename );
			FromStream( ifstr, true );
		}

		void ParamSet::UpdateMeanStd( const std::vector< ParamSet >& parsets )
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

		void ParamSet::UpdateMeanStd( const std::vector< double >& means, const std::vector< double >& stds )
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

		void ParamSet::PushNamePrefix( const String& prefix )
		{
			m_NamePrefixes.push_back( prefix );
		}

		void ParamSet::PopNamePrefix()
		{
			SCONE_ASSERT( m_NamePrefixes.size() > 0 );

			m_NamePrefixes.pop_back();
		}

		String ParamSet::GetNamePrefix() const
		{
			String full_prefix;
			for ( const String& s: m_NamePrefixes )
				full_prefix += s;
			return full_prefix;
		}

		std::ostream& ParamSet::ToStream( std::ostream& str ) const
		{
			for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
			{
				if ( iter->first.is_free )
					str << boost::format( "%-35s\t%14.8f\t%14.8f\t%14.8f\n" ) % iter->first.name % iter->second % iter->first.init_mean % iter->first.init_std;
			}

			return str;
		}

		std::istream& ParamSet::FromStream( std::istream& str, bool log_results )
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

			if ( log_results )
				log::InfoF( "Parameters read: %d, new parameters: %d", params_set, params_not_found );

			return str;
		}
	}
}
