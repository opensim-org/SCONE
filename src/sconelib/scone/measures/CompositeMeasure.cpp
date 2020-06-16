/*
** CompositeMeasure.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "CompositeMeasure.h"
#include "scone/core/Factories.h"
#include "scone/core/profiler_config.h"
#include "scone/core/Factories.h"
#include "scone/core/Log.h"
#include "xo/container/container_tools.h"
#include "scone/core/string_tools.h"

namespace scone
{
	CompositeMeasure::CompositeMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc ) :
		Measure( props, par, model, loc ),
		dual_sided( props.get_any<bool>( { "dual_sided", "symmetric" }, false ) ) // symmetric is for back. comp.
	{
		auto create_measure = [&]( const FactoryProps& fp ) {
			if ( dual_sided ) {
				m_Measures.push_back( CreateMeasure( fp, par, model, Location( LeftSide, true ) ) );
				m_Measures.push_back( CreateMeasure( fp, par, model, Location( RightSide, true ) ) );
			}
			else m_Measures.push_back( CreateMeasure( fp, par, model, loc ) );
		};

		// add any Measure
		for ( auto& m : props )
			if ( auto fp = MakeFactoryProps( GetMeasureFactory(), m, "Measure" ) )
				create_measure( fp );

		if ( Measures = props.try_get_child( "Measures" ) )
			for ( auto& m : *Measures )
				if ( auto fp = MakeFactoryProps( GetMeasureFactory(), m, "Measure" ) )
					create_measure( fp );

		// copy minimize flag from first measure
		INIT_PROP( props, minimize, !m_Measures.empty() ? m_Measures.front()->minimize : true );
	}

	void CompositeMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& m : m_Measures )
			m->StoreData( frame, flags );
	}

	bool CompositeMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION( model.GetProfiler() );

		bool terminate = false;
		for ( MeasureUP& m : m_Measures )
			terminate |= m->UpdateAnalysis( model, timestamp ) == true;

		return terminate ? true : false;
	}

	double CompositeMeasure::ComputeResult( const Model& model )
	{
		double total = 0.0;
		for ( MeasureUP& m : m_Measures )
		{
			double res_org = m->GetResult( model );
			double res_final = m->GetWeightedResult( model );
			total += res_final;
			bool hasweight = m->GetWeight() != 1;
			bool hasofs = m->GetOffset() != 0;
			bool hasthreshold = m->GetThreshold() != 0;
			string value = stringf( "%g", res_final );
			if ( hasweight || hasofs || hasthreshold )
			{
				value += " <- ";
				if ( hasweight )
					value += stringf( "%g * ", m->GetWeight() );
				if ( hasofs || hasthreshold )
					value += "(";
				value += stringf( "%g", res_org );
				if ( hasofs )
					value += stringf( " + %g", m->GetOffset() );
				if ( hasthreshold )
					value += stringf( " > %g", m->GetThreshold() );
				if ( hasofs || hasthreshold )
					value += ")";
			}
			GetReport().add_child( m->GetName(), m->GetReport() ).set_value( value );
		}

		GetReport().set_value( total );

		return total;
	}

	String CompositeMeasure::GetClassSignature() const
	{
		std::vector< String > strset;
		for ( auto& m : m_Measures )
		{
			string s = m->GetSignature();
			if ( xo::find( strset, s ) == strset.end() )
				strset.emplace_back( s );
		}
		return xo::concatenate_str( strset, "" );
	}
}
