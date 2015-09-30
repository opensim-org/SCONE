#include "stdafx.h"

#include "CompositeMeasure.h"
#include "../sim/Factories.h"
#include <boost/foreach.hpp>
#include "../core/Profiler.h"

namespace scone
{
	namespace cs
	{
		CompositeMeasure::Term::Term() :
		measure( nullptr )
		{
		}

		CompositeMeasure::Term::Term( const PropNode& pn ) :
		measure( nullptr ) // should be initialized by CompositeMeasure
		{
			INIT_PROPERTY_REQUIRED( pn, name );
			INIT_PROPERTY_REQUIRED( pn, weight );
			INIT_PROPERTY( pn, threshold, 0.0 );
			INIT_PROPERTY( pn, offset, 0.0 );
		}

		CompositeMeasure::Term::Term( Term&& other ) :
		name( other.name ),
		weight( other.weight ),
		threshold( other.threshold ),
		offset( other.offset ),
		measure( std::move( other.measure ) )
		{
		}

		CompositeMeasure::CompositeMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area )
		{
			const PropNode& termNode = props.GetChild( "Terms" );
			for ( auto it = termNode.Begin(); it != termNode.GetChildren().cend(); ++it )
			{
				Term t( *it->second );

				// first cast a ControllerUP to a Measure* using release(), because we don't have a CreateMeasure() factory
				Measure* m = dynamic_cast< Measure* >( sim::CreateController( it->second->GetChild( "Measure" ), par, model, area ).release() );
				SCONE_THROW_IF( m == nullptr, "Could not cast Controller* to Measure*" );
				t.measure = MeasureUP( m );

				m_Terms.push_back( std::move( t ) ); // use std::move because Term has a unique_ptr member
			}
		}

		void CompositeMeasure::StoreData( Storage< Real >::Frame& frame )
		{
			BOOST_FOREACH( Term& t, m_Terms )
				t.measure->StoreData( frame );
		}

		CompositeMeasure::~CompositeMeasure()
		{
		}

		sim::Controller::UpdateResult CompositeMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			bool terminate = false;
			BOOST_FOREACH( Term& t, m_Terms )
				terminate |= t.measure->UpdateAnalysis( model, timestamp ) == RequestTermination;

			return terminate ? RequestTermination : SuccessfulUpdate;
		}

		double CompositeMeasure::GetResult( sim::Model& model )
		{
			double total = 0.0;
			BOOST_FOREACH( Term& t, m_Terms )
			{
				double org_result = t.measure->GetResult( model );
				double ofset_result = org_result + t.offset;
				double thresh_result = ofset_result <= t.threshold ? 0.0 : ofset_result;
				double weighted_result = t.weight * thresh_result;

				log::DebugF( "%20s\t%8.3f\t%g * (%g + %g if > %g)", t.name.c_str(), weighted_result, t.weight, org_result, t.offset, t.threshold );
				total += weighted_result;

				m_Report.AddChild( t.name, t.measure->GetReport() ).Set( GetStringF( "%g\t%g * (%g + %g if > %g)", weighted_result, t.weight, org_result, t.offset, t.threshold ) );
			}

			log::DebugF( "%20s\t%8.3f", "TOTAL", total );
			m_Report.Set( total );

			return total;
		}

		scone::PropNode CompositeMeasure::GetReport()
		{
			return m_Report;
		}

		scone::String CompositeMeasure::GetClassSignature() const
		{
			String str;
			BOOST_FOREACH( auto& t, m_Terms )
				str += t.measure->GetSignature();

			return str;
		}
	}
}
