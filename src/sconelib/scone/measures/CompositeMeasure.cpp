/*
** CompositeMeasure.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "CompositeMeasure.h"
#include "scone/core/Factories.h"
#include "scone/core/Profiler.h"
#include "scone/core/Factories.h"
#include "scone/core/Log.h"

namespace scone
{
	CompositeMeasure::CompositeMeasure( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Measure( props, par, model, loc )
	{
		INIT_PROP( props, symmetric, false );

		auto create_measure = [&]( const PropNode& mpn ) {
			if ( symmetric ) {
				m_Measures.push_back( CreateMeasure( mpn, par, model, Location( LeftSide, true ) ) );
				m_Measures.push_back( CreateMeasure( mpn, par, model, Location( RightSide, true ) ) );
			}
			else m_Measures.push_back( CreateMeasure( mpn, par, model, loc ) );
		};

		// add any Measure
		for ( auto& m : props.select( "Measure" ) )
			create_measure( m.second );

		if (( Measures = props.try_get_child( "Measures" ) ))
			for ( auto& m : *Measures )
				create_measure( m.second );

		// copy minimize flag from first measure
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
		double total = 0.0;
		for ( MeasureUP& m : m_Measures )
		{
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
		std::vector< String > strset;
		for ( auto& m : m_Measures )
		{
			string s = m->GetSignature();
			if ( xo::find( strset, s ) == strset.end() )
				strset.emplace_back( s );
		}
		return xo::to_str( strset, "" );
	}
}
