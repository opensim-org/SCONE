#include "stdafx.h"

#include "CompositeMeasure.h"
#include "../sim/Factories.h"
#include <boost/foreach.hpp>

namespace scone
{
	namespace cs
	{
		CompositeMeasure::CompositeMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			const PropNode& termNode = props.GetChild( "Terms" );
			for ( auto it = termNode.Begin(); it != termNode.GetChildren().cend(); ++it )
			{
				Term t;

				// first cast a ControllerUP to a Measure* using release(), because we don't have a CreateMeasure() factory
				Measure* m = dynamic_cast< Measure* >( sim::CreateController( it->second->GetChild( "Measure" ), par, model, area ).release() );
				SCONE_CONDITIONAL_THROW( m == nullptr, "Could not cast Controller* to Measure*" );

				t.name = it->second->GetStr( "name", String("") );
				t.weight = it->second->GetReal( "weight" );
				t.measure = MeasureUP( m );

				m_Terms.push_back( std::move( t ) ); // use std::move because Term has a unique_ptr member
			}
		}

		CompositeMeasure::~CompositeMeasure()
		{
		}

		void CompositeMeasure::UpdateControls( sim::Model& model, double timestamp )
		{
			BOOST_FOREACH( Term& t, m_Terms )
			{
				t.measure->UpdateControls( model, timestamp );
				if ( t.measure->GetTerminationRequest() )
					SetTerminationRequest();
			}
		}

		double CompositeMeasure::GetResult( sim::Model& model )
		{
			double total = 0.0;
			BOOST_FOREACH( Term& t, m_Terms )
			{
				double unweighted_result = t.measure->GetResult( model );
				double weighted_result = t.weight * unweighted_result;

				log::DebugF( "%20s\t%8.3f\t(%8.3f * %8.3f)", t.name.c_str(), weighted_result, t.weight, unweighted_result );
				total += weighted_result;

				m_Report.AddChild( t.name, t.measure->GetReport() ).SetValue( GetStringF( "%g (%g * %g)", weighted_result, t.weight, unweighted_result ) );
			}

			m_Report.Set( total );

			return total;
		}

		scone::PropNode CompositeMeasure::GetReport()
		{
			return m_Report;
		}

		scone::String CompositeMeasure::GetSignature()
		{
			return "";
		}
	}
}
