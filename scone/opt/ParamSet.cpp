#include "stdafx.h"

#include "ParamSet.h"
#include "../core/Exception.h"
#include <algorithm>
#include <fstream>
#include <boost/format.hpp>
#include "../core/Log.h"
#include <boost/foreach.hpp>

namespace scone
{
	namespace opt
	{
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
			SCONE_THROW( "Invalid mode: " + ToString( m_Mode ) );
		}

		double ParamSet::Get( const String& name, const PropNode& props )
		{
			return Get( ParamInfo( GetNamePrefix() + name, props ) );
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

		void ParamSet::RestrainValues()
		{
			std::for_each( m_Params.begin(), m_Params.end(),
				[&]( std::pair< ParamInfo, double >& v ) { v.first.RestrainValue( v.second ); } );
		}

		void ParamSet::InitRandom()
		{
			for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
			{
				if ( iter->first.is_free )
					iter->second = iter->first.GetRandomValue();
			}
		}

		void ParamSet::Write( const String& filename )
		{
			std::ofstream ofstr( filename );

			for ( auto iter = m_Params.begin(); iter != m_Params.end(); ++iter )
			{
				if ( iter->first.is_free )
					ofstr << boost::format( "%-20s\t%16.8f\t%16.8f\t%16.8f\n" ) % iter->first.name % iter->second % iter->first.mean % iter->first.std;
			}
		}

		void ParamSet::Read( const String& filename )
		{
			std::ifstream ifstr( filename );

			size_t params_set = 0;
			size_t params_not_found = 0;
			size_t params_not_free = 0;

			while ( ifstr.good() )
			{
				std::string name;
				double value, mean, std;
				ifstr >> name >> value >> mean >> std;

				std::vector< std::pair< ParamInfo, double > >::iterator iter = FindParamByName( name );
				if ( iter != m_Params.end() )
				{
					// currently we only update free parameters (also for mean and std)
					if ( iter->first.is_free )
					{
						iter->second = value;
						iter->first.mean = mean;
						iter->first.std = std;
						++params_set;
					}
					else ++params_not_free;
				}
				else ++params_not_found;
			}

			// TODO: show statistics
			if ( params_set == 0 )
				SCONE_LOG( "Warning, no parameters were read from file" );
		}

		void ParamSet::UpdateMeanStd( const std::vector< ParamSet >& parsets )
		{
			for ( size_t parIdx = 0; parIdx < m_Params.size(); ++parIdx )
			{
				double weight = 1.0 / parsets.size();

				double mean = 0.0;
				for ( size_t setIdx = 0; setIdx < parsets.size(); ++setIdx )
					mean += weight * parsets[ setIdx ].m_Params[ parIdx ].second;

				double var = 0.0;
				for ( size_t setIdx = 0; setIdx < parsets.size(); ++setIdx )
					var += weight * Square( parsets[ setIdx ].m_Params[ parIdx ].second - mean );

				// update the result in ParInfo
				m_Params[ parIdx ].first.mean = mean;
				m_Params[ parIdx ].first.std = sqrt( var );
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
			BOOST_FOREACH( const String& s, m_NamePrefixes )
				full_prefix += s;
			return full_prefix;
		}
	}
}
