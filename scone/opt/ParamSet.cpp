#include "stdafx.h"

#include "ParamSet.h"
#include "../core/Exception.h"
#include <algorithm>

namespace scone
{
	namespace opt
	{
		double ParamSet::Get( const ParamInfo& info )
		{
			if ( m_Mode == CONSTRUCTION_MODE )
			{
				if ( FindParamByName( info.name ) != m_Params.end() )
					SCONE_THROW( "Duplicate parameter: " + info.name );

				// add parameter info
				m_Params.push_back( std::make_pair( info, info.GetRandomValue() ) );

				printf("%s: %.2f\n", info.name.c_str(), m_Params.back().second );

				return m_Params.back().second;
			}
			else if ( m_Mode == UPDATE_MODE )
			{
				auto iter = FindParamByName( info.name );
				if ( iter == m_Params.end() )
					SCONE_THROW( "Could not find parameter: " + info.name );

				// return parameter value
				return iter->second;
			}
			SCONE_THROW( "Invalid mode: " + ToString( m_Mode ) );
		}

		double ParamSet::GetMeanStd( const String& name, double init_mean, double init_std, double min, double max )
		{
			return Get( ParamInfo( name, init_mean, init_std, 0.0, 0.0, min, max ) );
		}

		double ParamSet::GetMinMax( const String& name, double init_min, double init_max, double min, double max )
		{
			return Get( ParamInfo( name, 0.0, 0.0, init_min, init_max, min, max ) );
		}

		double ParamSet::operator()( const String& name, double init_mean, double init_std, double min, double max )
		{
			return GetMeanStd( name, init_mean, init_std, min, max );
		}

		double ParamSet::operator()( const String& name, double min, double max )
		{
			return GetMinMax( name, min, max, min, max );
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
}
}
