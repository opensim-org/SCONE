#include "GaitCycleMeasure.h"
#include "scone/sim/Model.h"

namespace scone
{
	namespace cs
	{
		GaitCycleMeasure::GaitCycleMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			INIT_PROPERTY( props, use_half_cycle, false );

			m_InitState = model.GetStateValues();
			m_StateNames = model.GetStateVariableNames();
		}

		GaitCycleMeasure::~GaitCycleMeasure()
		{
		}

		double GaitCycleMeasure::GetResult( sim::Model& model )
		{
			return GetStateSimilarity( model.GetStateValues() );
		}

		Real GaitCycleMeasure::GetStateSimilarity( const std::vector< Real >& state )
		{
			Real total_diff = 0.0;
			for ( size_t idx = 0; idx < m_InitState.size(); ++idx )
			{
				String& name = m_StateNames[ idx ];

				// skip forward translation
				if ( name.rfind( "_tx" ) == name.size() - 3 )
					continue;

				Real diff = REAL_MAX;
				if ( use_half_cycle )
				{
					auto trg = GetMirroredStateNameAndSign( name );
					Index trg_idx = FindIndex( m_StateNames, trg.first );
					SCONE_ASSERT( trg_idx != NoIndex ); // make sure the target state name exists
					diff = abs( m_InitState[ idx ] - trg.second * state[ trg_idx ] );
				}
				else
				{
					diff = abs( m_InitState[ idx ] - state[ idx ] );
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
