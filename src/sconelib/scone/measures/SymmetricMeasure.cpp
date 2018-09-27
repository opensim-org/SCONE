#include "SymmetricMeasure.h"
#include "scone/model/Side.h"
#include "scone/core/Factories.h"
#include "scone/core/string_tools.h"
#include "../core/Profiler.h"
#include <set>
#include "../core/Log.h"

namespace scone
{
	SymmetricMeasure::SymmetricMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		for ( auto& m : props.select( "Measure" ) )
		{
			m_Measures.push_back( CreateMeasure( m.second, par, model, Location( LeftSide, true ) ) );
			m_Measures.push_back( CreateMeasure( m.second, par, model, Location( RightSide, true ) ) );
		}
	}

	double SymmetricMeasure::ComputeResult( Model& model )
	{
		log::trace( "Getting results of ", GetName() );
		double total = 0.0;
		for ( MeasureUP& m : m_Measures )
		{
			double res_org = m->GetResult( model );
			double res_final = m->GetWeightedResult( model );
			total += res_final;
			GetReport().push_back( m->GetName(), m->GetReport() ).set_value( stringf( "%g\t%g * %g ", res_final, m->GetWeight(), res_org ) );
		}

		GetReport().set_value( total );

		return total;
	}

	bool SymmetricMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		bool terminate = false;
		for ( MeasureUP& m : m_Measures )
			terminate |= m->UpdateAnalysis( model, timestamp ) == true;

		return terminate ? true : false;
	}

	String SymmetricMeasure::GetClassSignature() const
	{
		std::set< String > strset;
		for ( auto& m : m_Measures )
			strset.insert( m->GetSignature() );
		return xo::to_str( strset, "." );
	}
}
