#include "stdafx.h"

#include "MuscleReflex.h"
#include "scone/sim/Muscle.h"
#include "scone/sim/Area.h"
#include "Tools.h"
#include "scone/sim/Dof.h"

//#define DEBUG_MUSCLE "vasti_r"

namespace scone
{
	namespace cs
	{
		MuscleReflex::MuscleReflex( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		Reflex( props, par, model, area ),
		m_pForceSensor( nullptr ),
		m_pLengthSensor( nullptr ),
		m_pVelocitySensor( nullptr )
		{
			sim::Muscle& source = *FindByName( model.GetMuscles(), props.GetStr( "source", props.GetStr( "target" ) ) + GetSideName( area.side ) );

			// init names
			String reflexname = GetReflexName( m_Target.GetName(), source.GetName() );
			opt::ScopedParamSetPrefixer prefixer( par, reflexname + "." );

			INIT_PARAM_NAMED( props, par, length_gain, "KL", 0.0 );
			INIT_PARAM_NAMED( props, par, length_ofs, "L0", 1.0 );
			INIT_PARAM_NAMED( props, par, u_constant, "C", 0.0 );
			INIT_PARAM_NAMED( props, par, velocity_gain, "KV", 0.0 );
			INIT_PARAM_NAMED( props, par, force_gain, "KF", 0.0 );

			// create delayed sensors
			if ( force_gain != 0.0 )
				m_pForceSensor = &model.AcquireDelayedSensor< sim::MuscleForceSensor >( source );

			if ( length_gain != 0.0 )
				m_pLengthSensor = &model.AcquireDelayedSensor< sim::MuscleLengthSensor >( source );

			if ( velocity_gain != 0.0 )
				m_pVelocitySensor = &model.AcquireDelayedSensor< sim::MuscleVelocitySensor >( source );
		}

		MuscleReflex::~MuscleReflex()
		{
		}

		void MuscleReflex::ComputeControls( double timestamp )
		{
			// add stretch reflex
			Real u_l = m_pLengthSensor ? length_gain * ( m_pLengthSensor->GetValue( delay ) - length_ofs ) : 0;

			// add velocity reflex
			// TODO: should velocity gain be positive only?
			Real u_v = m_pVelocitySensor ? velocity_gain * std::max( 0.0, m_pVelocitySensor->GetValue( delay ) ) : 0;

			// add force reflex
			Real u_f = m_pForceSensor ? force_gain * m_pForceSensor->GetValue( delay ) : 0;

			Real u_total = u_l + u_v + u_f + u_constant;

			m_Target.AddControlValue( std::max( 0.0, u_total ) );

#ifdef DEBUG_MUSCLE
			if ( m_Target.GetName() == DEBUG_MUSCLE )
				log::TraceF( "u_l=%.3f u_v=%.3f u_f=%.3f", u_l, u_v, u_f );
#endif
		}
	}
}
