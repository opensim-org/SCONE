#include "stdafx.h"
#include "GaitCycleMeasure.h"
#include "../sim/Model.h"
#include "boost/foreach.hpp"

namespace scone
{
	namespace cs
	{
		GaitCycleMeasure::GaitCycleMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			INIT_FROM_PROP( props, use_half_cycle, false );

			m_InitState = model.GetState();

			for ( auto iter = m_InitState.begin(); iter != m_InitState.end(); ++iter )
			{
				if ( iter->first.rfind( "_tx" ) == iter->first.size() - 3 )
					iter = m_InitState.erase( iter );
			}
		}

		GaitCycleMeasure::~GaitCycleMeasure()
		{

		}

		void GaitCycleMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
		}

		double GaitCycleMeasure::GetResult( sim::Model& model )
		{
			return GetStateSimilarity( model.GetState() );
		}

		Real GaitCycleMeasure::GetStateSimilarity( sim::State& state )
		{
			Real total_diff = 0.0;
			BOOST_FOREACH( auto& nvp, m_InitState )
			{
				Real diff = REAL_MAX;
				if ( use_half_cycle )
				{
					auto trg = GetMirroredStateNameAndSign( nvp.first );
					SCONE_ASSERT( state.count( trg.first ) > 0 ); // make sure the target state name exists
					diff = abs( nvp.second - trg.second * state[ trg.first ] );
				}
				else
				{
					diff = abs( nvp.second * state[ nvp.first ] );
				}

				total_diff += diff * diff;
			}

			return total_diff;
		}

		std::pair< String, int > GaitCycleMeasure::GetMirroredStateNameAndSign( const String& str )
		{
			size_t extPos = str.find_last_of( '_' );
			String baseName = str.substr( 0, extPos );
			String ext = extPos != String::npos ? str.substr( extPos + 1 ) : "";
			if ( ext == "r" )
				return std::make_pair( baseName + "l", 1 );
			else if ( ext == "l" )
				return std::make_pair( baseName + "r", 1 );
			else if ( ext == "list" || ext == "rotation" || ext == "bending" )
				return std::make_pair( str, -1 ); // swap signs for these types

			// no need to mirror this state
			return std::make_pair( str, 1 );
		}

	}
}
