#include "EffortMeasure.h"
#include "scone/sim/Model.h"
#include "scone/sim/sim.h"
#include "scone/sim/Muscle.h"
#include "scone/core/Profiler.h"

namespace scone
{
	namespace cs
	{
		StringMap< EffortMeasure::EnergyMeasureType > EffortMeasure::m_MeasureNames = StringMap< EffortMeasure::EnergyMeasureType >(
			EffortMeasure::TotalForce, "TotalForce",
			EffortMeasure::Wang2012, "Wang2012",
			EffortMeasure::Constant, "Constant"
			);

		EffortMeasure::EffortMeasure( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Measure( props, par, model, area ),
		m_Energy( Statistic<>::LinearInterpolation )
		{
			measure_type = m_MeasureNames.GetValue( props.GetStr( "measure_type" ) );
			INIT_PROPERTY( props, use_cost_of_transport, false );
            INIT_PROPERTY( props, specific_tension, 0.25e6 );
            INIT_PROPERTY( props, muscle_density, 1059.7 );

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

			// make sure this is a new step and the measure is active
			SCONE_ASSERT( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() );
			if ( !IsActive( model, timestamp ) )
				return NoUpdate;

			double current_effort = GetEnergy( model );
			m_Energy.AddSample( timestamp, current_effort );

			return SuccessfulUpdate;
		}

		double EffortMeasure::GetResult( sim::Model& model )
		{
			double distance = std::max( 0.01, model.GetComPos().x - m_InitComPos.x );
			double cot = m_Energy.GetTotal() / ( model.GetMass() * distance );

			GetReport().Add( "cost_of_transport", cot );
			GetReport().Add( "average", m_Energy.GetAverage() );
			GetReport().Add( "total", m_Energy.GetTotal() );
			GetReport().Add( "probe_total", model.GetTotalEnergyConsumption() );

			if ( use_cost_of_transport )
				return cot;
			else return m_Energy.GetAverage();
		}

		double EffortMeasure::GetEnergy( const sim::Model& model ) const
		{
			switch( measure_type )
			{
			case TotalForce: return GetTotalForce( model );
			case Wang2012: return GetWang2012( model );
			case Constant: return model.GetMass();
			default: SCONE_THROW( "Invalid energy measure" );
			}
		}

		double EffortMeasure::GetTotalForce( const sim::Model& model ) const
		{
			double f = 1.0; // base muscle force
			for ( const sim::MuscleUP& mus: model.GetMuscles() )
				f += mus->GetForce();

			return f;
		}

		double EffortMeasure::GetWang2012( const sim::Model& model ) const
		{
			double e = m_Wang2012BasalEnergy;
			for ( const sim::MuscleUP& mus: model.GetMuscles() )
			{
				double mass = mus->GetMass( specific_tension, muscle_density);
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

		scone::String EffortMeasure::GetClassSignature() const
		{
			String s;

			if ( use_cost_of_transport )
				s += "C";
			else s += "A";

			switch( measure_type )
			{
			case TotalForce: s += "F"; break;
			case Wang2012: s += "W"; break;
			case Constant: s += "C"; break;
			default: SCONE_THROW( "Invalid energy measure" );
			}

			return s;
		}
	}
}
