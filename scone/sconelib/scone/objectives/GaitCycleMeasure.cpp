#include "GaitCycleMeasure.h"
#include "scone/model/Model.h"

namespace scone
{
	GaitCycleMeasure::GaitCycleMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area )
	{
		INIT_PROPERTY( props, use_half_cycle, false );

		m_InitState = model.GetState();
	}

	GaitCycleMeasure::~GaitCycleMeasure()
	{
	}

	double GaitCycleMeasure::GetResult( Model& model )
	{
		return GetStateSimilarity( model.GetState() );
	}

	Real GaitCycleMeasure::GetStateSimilarity( const State& state )
	{
		Real total_diff = 0.0;
		for ( size_t idx = 0; idx < m_InitState.GetSize(); ++idx )
		{
			auto& name = m_InitState.GetName( idx );

			// skip forward translation
			if ( name.rfind( "_tx" ) == name.size() - 3 )
				continue;

			Real diff = REAL_MAX;
			if ( use_half_cycle )
			{
				auto trg = GetMirroredStateNameAndSign( name );
				Index trg_idx = FindIndex( state.GetNames(), trg.first );
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
