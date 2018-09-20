#include "DofReflex.h"
#include "scone/model/Dof.h"
#include "scone/model/Side.h"
#include "scone/model/Actuator.h"
#include "scone/model/Model.h"
#include "scone/model/SensorDelayAdapter.h"
#include "scone/model/Sensors.h"

namespace scone
{
	DofReflex::DofReflex( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Reflex( props, par, model, loc ),
	source( props.get< String >( "source" ) ),
	m_SourceDof( *FindByNameTrySided( model.GetDofs(), source, loc.side ) ),
	m_SourceParentDof( props.has_key( "source_parent" ) ? &*FindByNameTrySided( model.GetDofs(), props.get< String >( "source_parent" ), loc.side ) : nullptr ),
	m_DelayedPos( model.AcquireDelayedSensor< DofPositionSensor >( m_SourceDof, m_SourceParentDof ) ),
	m_DelayedVel( model.AcquireDelayedSensor< DofVelocitySensor >( m_SourceDof, m_SourceParentDof ) ),
	m_pTargetPosSource( nullptr )
	{
		String par_name = GetParName( props );
		name = GetReflexName( m_Target.GetName(), source );
		ScopedParamSetPrefixer prefixer( par, par_name + "." );

		INIT_PAR_NAMED( props, par, P0, "P0", 0.0 );
		INIT_PAR_NAMED( props, par, V0, "V0", 0.0 );
		INIT_PAR_NAMED( props, par, KP, "KP", 0.0 );
		INIT_PAR_NAMED( props, par, KV, "KV", 0.0 );
		INIT_PAR_NAMED( props, par, C0, "C0", 0.0 );
		INIT_PROP( props, condition, 0 );
		INIT_PROP( props, filter_cutoff_frequency, 0.0 );

		if ( filter_cutoff_frequency != 0.0 )
			m_Filter = xo::make_lowpass_butterworth_2nd_order( model.GetSimulationStepSize() * filter_cutoff_frequency );

		if ( auto p0pn = props.try_get< String >( "P0_source" ) )
			m_pTargetPosSource = &model.AcquireDelayedSensor< DofPositionSensor >( *FindByNameTrySided( model.GetDofs(), *p0pn, loc.side ) );
	}

	DofReflex::~DofReflex()
	{
	}

	void DofReflex::ComputeControls( double timestamp )
	{
		Real pos = m_DelayedPos.GetValue( delay );
		Real vel = m_DelayedVel.GetValue( delay );

		if ( filter_cutoff_frequency != 0.0 )
		{
			pos = m_Filter( pos );
			vel = m_Filter.velocity() * 1000;
		}

		auto delta_pos = P0 - pos;
		auto delta_vel = V0 - vel;

		if ( condition == 0 || ( condition == -1 && delta_pos < 0 && delta_vel < 0 ) || condition == 1 && delta_pos > 0 && delta_vel > 0 )
		{
			u_p = KP * delta_pos;
			u_d = KV * delta_vel;
			AddTargetControlValue( C0 + u_p + u_d );
		}
		else u_p = u_d = 0.0;
	}

	void DofReflex::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		frame[ name + ".RDP" ] = u_p;
		frame[ name + ".RDV" ] = u_d;
	}
}
