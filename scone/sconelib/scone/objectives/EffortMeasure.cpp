#include "EffortMeasure.h"
#include "scone/model/Model.h"
#include "scone/model/Muscle.h"
#include "scone/core/Profiler.h"
#include "scone/core/math.h"
#include "flut/pattern_matcher.hpp"

namespace scone
{
	StringMap< EffortMeasure::EnergyMeasureType > EffortMeasure::m_MeasureNames = StringMap< EffortMeasure::EnergyMeasureType >(
		EffortMeasure::TotalForce, "TotalForce",
		EffortMeasure::Wang2012, "Wang2012",
		EffortMeasure::Constant, "Constant",
		EffortMeasure::Uchida2016, "Uchida2016"
		);

	EffortMeasure::EffortMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		m_Energy( Statistic<>::LinearInterpolation )
	{
		measure_type = m_MeasureNames.GetValue( props.get< String >( "measure_type" ) );
		INIT_PROPERTY( props, use_cost_of_transport, false );
		INIT_PROPERTY( props, specific_tension, 0.25e6 );
		INIT_PROPERTY( props, muscle_density, 1059.7 );
		INIT_PROPERTY( props, default_muscle_slow_twitch_ratio, 0.5 );
		INIT_PROPERTY( props, use_symmetric_fiber_ratios, true );

		// precompute some stuff
		m_Wang2012BasalEnergy = 1.51 * model.GetMass();
		m_Uchida2016BasalEnergy = 1.2 * model.GetMass();
		m_AerobicFactor = 1.5; // 1.5 is for aerobic conditions, 1.0 for anaerobic. may need to add as option later
		m_InitComPos = model.GetComPos();
		SetSlowTwitchRatios( props, model );

	}

	EffortMeasure::MuscleProperties::MuscleProperties( const PropNode& props ) :
		muscle( props.get< String >( "muscle" ) )
	{
		Real ratio = props.get< Real >( "slow_twitch_ratio" );
		SCONE_ASSERT_MSG( ( ratio >= 0.0 && ratio <= 1.0 ), "slow_twitch_ratios must be between 0.0 and 1.0" );
		slow_twitch_ratio = ratio;
	}

	EffortMeasure::~EffortMeasure()
	{
	}

	Controller::UpdateResult EffortMeasure::UpdateAnalysis( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		// make sure this is a new step and the measure is active
		SCONE_ASSERT( model.GetIntegrationStep() != model.GetPreviousIntegrationStep() );
		if ( !IsActive( model, timestamp ) )
			return NoUpdate;

		double current_effort = GetEnergy( model );
		m_Energy.AddSample( timestamp, current_effort );

		return SuccessfulUpdate;
	}

	double EffortMeasure::GetResult( Model& model )
	{
		double distance = std::max( 0.01, model.GetComPos().x - m_InitComPos.x );
		double cot = m_Energy.GetTotal() / ( model.GetMass() * distance );

		GetReport().set( "cost_of_transport", cot );
		GetReport().set( "average", m_Energy.GetAverage() );
		GetReport().set( "total", m_Energy.GetTotal() );
		GetReport().set( "distance", distance );
		GetReport().set( "speed", distance / model.GetTime() );
		GetReport().set( "probe_total", model.GetTotalEnergyConsumption() );

		if ( use_cost_of_transport )
			return cot;
		else return m_Energy.GetAverage();
	}

	double EffortMeasure::GetEnergy( const Model& model ) const
	{
		switch ( measure_type )
		{
		case TotalForce: return GetTotalForce( model );
		case Wang2012: return GetWang2012( model );
		case Constant: return model.GetMass();
		case Uchida2016: return GetUchida2016( model );
		default: SCONE_THROW( "Invalid energy measure" );
		}
	}

	double EffortMeasure::GetTotalForce( const Model& model ) const
	{
		double f = 1.0; // base muscle force
		for ( const MuscleUP& mus : model.GetMuscles() )
			f += mus->GetForce();

		return f;
	}

	double EffortMeasure::GetWang2012( const Model& model ) const
	{
		double e = m_Wang2012BasalEnergy;
		for ( Index i = 0; i < model.GetMuscles().size(); ++i )
		{
			const MuscleUP& mus = model.GetMuscles()[ i ];
			double mass = mus->GetMass( specific_tension, muscle_density );
			Real l = m_SlowTwitchFiberRatios[ i ];
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
			Real effort_s = ( -v_ce > 0 ) ? 0.25 * mus->GetForce() * -v_ce : 0.0;
			Real effort_w = ( -v_ce > 0 ) ? mus->GetActiveFiberForce() * -v_ce : 0.0;
			Real effort = effort_a + effort_m + effort_s + effort_w;

			e += effort;
		}

		return e;
	}

	// Implementation of Umberger (2003, 2010) metabolics model 
	// with updates from Uchida 2016.
	double EffortMeasure::GetUchida2016( const Model& model ) const
	{
		double e = m_Uchida2016BasalEnergy;
		for ( Index i = 0; i < model.GetMuscles().size(); ++i )
		{
			const MuscleUP& mus = model.GetMuscles()[ i ];
			double mass = mus->GetMass( specific_tension, muscle_density );

			// calculate A parameter
			Real excitation = mus->GetExcitation();
			Real activation = mus->GetActivation();
			double A;
			if ( excitation > activation )
				A = excitation;
			else
				A = ( excitation + activation ) / 2;

			// calculate slowTwitchRatio factor
			Real slowTwitchRatio = m_SlowTwitchFiberRatios[ i ];
			double uSlow = slowTwitchRatio * sin( REAL_HALF_PI * excitation );
			double uFast = ( 1 - slowTwitchRatio ) * ( 1 - cos( REAL_HALF_PI * excitation ) );
			slowTwitchRatio = ( excitation == 0 ) ? 1.0 : uSlow / ( uSlow + uFast );

			// calculate AMdot
			double AMdot;
			double unscaledAMdot = 128 * ( 1 - slowTwitchRatio ) + 25;
			double F_iso = mus->GetActiveForceLengthMultipler();
			if ( mus->GetNormalizedFiberLength() <= 1.0 )
				AMdot = m_AerobicFactor * std::pow( A, 0.6 ) * unscaledAMdot;
			else
				AMdot = m_AerobicFactor * std::pow( A, 0.6 ) * ( ( 0.4 * unscaledAMdot ) + ( 0.6 * unscaledAMdot * F_iso ) );

			// calculate shortening heat rate
			double Sdot;
			double Vmax_fasttwitch = mus->GetMaxContractionVelocity();
			double Vmax_slowtwitch = mus->GetMaxContractionVelocity() / 2.5;
			double alpha_shortening_fasttwitch = 153 / Vmax_fasttwitch;
			double alpha_shortening_slowtwitch = 100 / Vmax_slowtwitch;
			double fiber_velocity_normalized = mus->GetFiberVelocity() / mus->GetOptimalFiberLength();
			double unscaledSdot, tmp_slowTwitch, tmp_fastTwitch;

			if ( fiber_velocity_normalized <= 0 )
			{
				double maxShorteningRate = 100.0; // (W/kg)
				tmp_slowTwitch = -alpha_shortening_slowtwitch * fiber_velocity_normalized;
				if ( tmp_slowTwitch > maxShorteningRate ) tmp_slowTwitch = maxShorteningRate;

				tmp_fastTwitch = alpha_shortening_fasttwitch * fiber_velocity_normalized * ( 1 - slowTwitchRatio );
				unscaledSdot = ( tmp_slowTwitch * slowTwitchRatio ) - tmp_fastTwitch;
				Sdot = m_AerobicFactor * A * A * unscaledSdot;
			}
			else
			{
				unscaledSdot = 4.0 * alpha_shortening_slowtwitch * fiber_velocity_normalized;
				Sdot = m_AerobicFactor * A * unscaledSdot;
			}

			if ( mus->GetNormalizedFiberLength() > 1.0 ) Sdot *= F_iso;

			double active_fiber_force = mus->GetActiveFiberForce();
			if ( active_fiber_force < 0 ) active_fiber_force = 0;

			// calculate mechanical work rate
			double Wdot =
				-active_fiber_force * mus->GetFiberVelocity() / mass;

			// prevent instantaneous negative power by accounting for it through Sdot
			double Edot_Wkg_beforeClamp = AMdot + Sdot + Wdot;
			if ( Edot_Wkg_beforeClamp < 0 ) Sdot -= Edot_Wkg_beforeClamp;

			// total heat rate cannot fall below 1.0 W/kg
			double totalHeatRate = AMdot + Sdot;
			if ( totalHeatRate < 1.0 ) totalHeatRate = 1.0;

			// total metabolic rate for this muscle
			double Edot = ( totalHeatRate + Wdot ) * mass;

			e += Edot;
		}
		return e;
	}

	void EffortMeasure::SetSlowTwitchRatios( const PropNode& props, const Model& model )
	{
		// initialize all muscles to default
		std::vector< Real > init( model.GetMuscles().size(), default_muscle_slow_twitch_ratio );
		m_SlowTwitchFiberRatios = init;

		// read in fiber ratios. throw exception if out of [0,1] range
		//std::map< String, Real > fiberRatioMap;
		std::vector< MuscleProperties > muscPropsInput;
		if ( const PropNode* muscleProperties = props.try_get_child( "MuscleProperties" ) )
		{
			for ( auto it = muscleProperties->begin(); it != muscleProperties->end(); ++it )
				muscPropsInput.emplace_back( MuscleProperties( it->second ) );
		}

		// update muscle if its name is in the map
		for ( Index i = 0; i < model.GetMuscles().size(); ++i )
		{
			const MuscleUP& mus = model.GetMuscles()[ i ];

			bool foundMuscle = false;
			for ( auto it = muscPropsInput.begin(); it != muscPropsInput.end(); ++it )
			{
				if ( flut::pattern_matcher( it->muscle, ";" )( mus->GetName() ) )
				{
					SCONE_ASSERT_MSG( !foundMuscle, "multiple muscle names matched in MuscleProperties" );
					m_SlowTwitchFiberRatios[ i ] = it->slow_twitch_ratio;
					foundMuscle = true;
				}
			}
		}
	}

	scone::String EffortMeasure::GetClassSignature() const
	{
		String s;

		if ( use_cost_of_transport )
			s += "C";
		else s += "A";

		switch ( measure_type )
		{
		case TotalForce: s += "F"; break;
		case Wang2012: s += "W"; break;
		case Constant: s += "C"; break;
		case Uchida2016: s += "U"; break;
		default: SCONE_THROW( "Invalid energy measure" );
		}

		return s;
	}

	void EffortMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		frame[ "metabolics_penalty" ] = m_Energy.GetLatest();
	}
}
