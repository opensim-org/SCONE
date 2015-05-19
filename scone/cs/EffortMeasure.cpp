#include "stdafx.h"
#include "EffortMeasure.h"
#include "boost/foreach.hpp"
#include "../sim/Model.h"
#include "../sim/sim.h"
#include "../sim/Muscle.h"

#include <boost/assign.hpp>
#include "../core/Profiler.h"

namespace scone
{
	namespace cs
	{
		StringMap< EffortMeasure::EnergyMeasureType > EffortMeasure::m_MeasureNames = StringMap< EffortMeasure::EnergyMeasureType >(
			EffortMeasure::TotalForce, "TotalForce",
			EffortMeasure::Wang2012, "Wang2012"
			);

		EffortMeasure::EffortMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_Energy( Statistic<>::LinearInterpolation )
		{
			measure_type = m_MeasureNames.GetValue( props.GetStr( "measure_type" ) );
			INIT_PROPERTY( props, use_cost_of_transport, false );

			// precompute some stuff
			m_Wang2012BasalEnergy = 1.51 * model.GetMass();
			m_InitComPos = model.GetComPos();
		}

		EffortMeasure::~EffortMeasure()
		{
		}

		sim::Controller::UpdateResult EffortMeasure::UpdateAnalysis( const sim::Model& model, double timestamp )
		{
			SCONE_PROFILE_SCOPE;

			// make sure this is a new step
			if ( model.GetIntegrationStep() == model.GetPreviousIntegrationStep() )
				return NoUpdate;

			double current_effort = GetEnergy( model );
			m_Energy.AddSample( timestamp, current_effort );

			return SuccessfulUpdate;
		}

		double EffortMeasure::GetResult( sim::Model& model )
		{
			if ( use_cost_of_transport )
			{
				double distance = std::max( 0.01, model.GetComPos().x - m_InitComPos.x );
				return m_Energy.GetTotal() / ( model.GetMass() * distance );
			}
			else return m_Energy.GetTotal();
		}

		double EffortMeasure::GetEnergy( const sim::Model& model ) const
		{
			switch( measure_type )
			{
			case TotalForce: return GetTotalForce( model );
			case Wang2012: return GetWang2012( model );
			default: SCONE_THROW( "Invalid energy measure" );
			}
		}

		double EffortMeasure::GetTotalForce( const sim::Model& model ) const
		{
			double f = 1.0; // base muscle force
			BOOST_FOREACH( const sim::MuscleUP& mus, model.GetMuscles() )
				f += mus->GetForce();

			return f;
		}

		double EffortMeasure::GetWang2012( const sim::Model& model ) const
		{
			double e = m_Wang2012BasalEnergy;
			BOOST_FOREACH( const sim::MuscleUP& mus, model.GetMuscles() )
			{
				double mass = mus->GetMass();
				Real l = 0.5;
				Real fa = 40 * l * sin( REAL_HALF_PI * mus->GetExcitation() ) + 133 * ( 1 - l ) * ( 1 - cos( REAL_HALF_PI * mus->GetExcitation() ) );
				Real fm = 74 * l * sin( REAL_HALF_PI * mus->GetActivation() ) + 111 * ( 1 - l ) * ( 1 - cos( REAL_HALF_PI * mus->GetActivation() ) );
				Real l_ce_norm = mus->GetFiberLength() / mus->GetOptimalFiberLength();
				Real v_ce = mus->GetFiberVelocity();
				Real g = 0.0;
				if ( l_ce_norm < 0.5 )
					g = 0.5;
				else if ( l_ce_norm < 1.0 )
					g = l_ce_norm;
				else if ( l_ce_norm < 1.5 )
					g = -2 * l_ce_norm + 3;

				Real effort_a = mass * fa;
				Real effort_m = mass * g * fm;
				Real effort_s = (-v_ce > 0 ) ? 0.25 * mus->GetForce() * -v_ce : 0.0;
				Real effort_w = (-v_ce > 0 ) ? mus->GetFiberForce() * -v_ce : 0.0;
				Real effort = effort_a + effort_m + effort_s + effort_w;

				e += effort;
			}

			return e;
		}

		scone::String EffortMeasure::GetMainSignature() const
		{
			switch( measure_type )
			{
			case TotalForce: return "F";
			case Wang2012: return "W";
			default: SCONE_THROW( "Invalid energy measure" );
			}
		}
	}
}
