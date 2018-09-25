#include "CompositeMeasure.h"
#include "scone/core/Factories.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"
#include "../core/Log.h"

namespace scone
{
	CompositeMeasure::CompositeMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc ),
	Measures( props.try_get_child( "Measures" ) )
	{
		INIT_PROP( props, symmetric, false );

		if ( !Measures )
			Measures = &props;

		// add any Measure
		for ( auto& m : Measures->select( "Measure" ) )
		{
			if ( symmetric )
			{
				m_Measures.push_back( CreateMeasure( m.second, par, model, Location( LeftSide, true ) ) );
				m_Measures.push_back( CreateMeasure( m.second, par, model, Location( RightSide, true ) ) );
			}
			else m_Measures.push_back( CreateMeasure( m.second, par, model, loc ) );
		}

		// copy minimize flag from
		INIT_PROP( props, minimize, !m_Measures.empty() ? m_Measures.front()->minimize : true );
	}

	void CompositeMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& m : m_Measures )
			m->StoreData( frame, flags );
	}

	CompositeMeasure::~CompositeMeasure() { }

	bool CompositeMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		bool terminate = false;
		for ( MeasureUP& m : m_Measures )
			terminate |= m->UpdateAnalysis( model, timestamp ) == true;

		return terminate ? true : false;
	}

	double CompositeMeasure::ComputeResult( Model& model )
	{
		log::trace( "Getting results of ", GetName() );
		double total = 0.0;
		for ( MeasureUP& m : m_Measures )
		{
			log::trace( "Getting Composite results of ", m->GetName() );
			double res_org = m->GetResult( model );
			double res_final = m->GetWeightedResult( model );
			total += res_final;

			GetReport().push_back( m->GetName(), m->GetReport() ).set_value( stringf( "%g\t%g * (%g + %g if > %g)", res_final, m->GetWeight(), res_org, m->GetOffset(), m->GetThreshold() ) );
		}

		GetReport().set_value( total );

		return total;
	}

	scone::String CompositeMeasure::GetClassSignature() const
	{
		std::set< String > strset;
		for ( auto& m : m_Measures )
			strset.insert( m->GetSignature() );
		return xo::concatenate_str( strset );
	}
}
