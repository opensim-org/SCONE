
#include "CompositeMeasure.h"
#include "scone/core/Factories.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"

namespace scone
{
	CompositeMeasure::CompositeMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Measure( props, par, model, area ),
	Measures( props.try_get_child( "Measures" ) )
	{
		if ( Measures ) // old style, with special group
		{
			for ( auto& m : *Measures )
				m_Measures.push_back( CreateMeasure( m.second, par, model, area ) );
		}

		// add any Measure
		for ( auto& m : props.select( "Measure" ) )
			m_Measures.push_back( CreateMeasure( m.second, par, model, area ) );

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

		// update Measures
		for ( MeasureUP& m : m_Measures )
			terminate |= m->UpdateAnalysis( model, timestamp ) == true;

		return terminate ? true : false;
	}

	double CompositeMeasure::GetResult( Model& model )
	{
		double total = 0.0;
		for ( MeasureUP& m : m_Measures )
		{
			double org_result = m->GetResult( model );
			double ofset_result = org_result + m->GetOffset();
			double thresh_result = ofset_result <= m->GetThreshold() ? 0.0 : ofset_result;
			double weighted_result = m->GetWeight() * thresh_result;

			total += weighted_result;

			GetReport().push_back( m->GetName(), m->GetReport() ).set_value( stringf( "%g\t%g * (%g + %g if > %g)", weighted_result, m->GetWeight(), org_result, m->GetOffset(), m->GetThreshold() ) );
		}

		GetReport().set_value( total );

		return total;
	}

	scone::String CompositeMeasure::GetClassSignature() const
	{
		String str;
		for ( auto& m : m_Measures )
			str += m->GetSignature();

		return str;
	}
}
