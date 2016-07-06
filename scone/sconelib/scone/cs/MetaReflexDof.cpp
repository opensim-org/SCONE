#include "MetaReflexDof.h"
#include "scone/sim/Area.h"
#include "scone/sim/Dof.h"
#include "scone/core/propnode_tools.h"
#include "scone/sim/Muscle.h"
#include "scone/sim/Link.h"
#include "scone/sim/Joint.h"
#include "scone/sim/SensorDelayAdapter.h"

//#define DEBUG_MUSCLE "hamstrings_r"

namespace scone
{
	namespace cs
	{
		MetaReflexDof::MetaReflexDof( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		target_dof( *FindByName( model.GetDofs(), props.GetStr( "target" ) + GetSideName( area.side ) ) ),
		tot_available_pos_mom( 0.0 ),
		tot_available_neg_mom( 0.0 ),
		local_balance( 0 ),
		body_angvel_sensor_gain( model.balance_sensor_ori_vel_gain ),
		body_sensor_delay( model.balance_sensor_delay ),
		body_ori_sensor( nullptr ),
		body_angvel_sensor( nullptr )
		{
			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "MetaReflex" );

			opt::ScopedParamSetPrefixer prefixer( par, props.GetStr( "target" ) + "." );

			// init dof parameters
			dof_pos.Init( props, par, model, "P" );
			dof_neg.Init( props, par, model, "N" );

			if ( model.GetCustomProp( "meta_reflex_control.use_balance", true ) && props.HasKey( "Balance" ) )
			{
				opt::ScopedParamSetPrefixer pre2( par, "B." );
				auto& balprops = props.GetChild( "Balance" );
				auto& body = FindByName( model.GetBodies(), balprops.GetStr( "body" ) );

				// create sensors
				body_ori_sensor = &model.AcquireDelayedSensor< sim::BodyOriSensor >( *body );
				body_angvel_sensor = &model.AcquireDelayedSensor< sim::BodyAngVelSensor >( *body );

				bal_pos.Init( balprops, par, model, "P" );
				bal_neg.Init( balprops, par, model, "N" );

				INIT_PARAM_NAMED( balprops, par, body_angvel_sensor_gain, "KAV", model.balance_sensor_ori_vel_gain );
			}

			// TODO: move to muscle
			INIT_PROPERTY_REQUIRED( props, delay );
		}

		void MetaReflexDof::AddAvailableMoment( Real max_moment )
		{
			if ( max_moment > 0 )
				tot_available_pos_mom += max_moment;
			else tot_available_neg_mom += max_moment; // neg moment stays negative
		}

		void MetaReflexDof::UpdateLocalBalance( const Vec3& global_balance )
		{
			if ( body_ori_sensor && body_angvel_sensor )
			{
				Vec3 glob_ori = Vec3( body_ori_sensor->GetValue( 0u, body_sensor_delay ), body_ori_sensor->GetValue( 1u, body_sensor_delay ), body_ori_sensor->GetValue( 2u, body_sensor_delay ) );
				Vec3 glob_angvel = Vec3( body_angvel_sensor->GetValue( 0u, body_sensor_delay ), body_angvel_sensor->GetValue( 1u, body_sensor_delay ), body_angvel_sensor->GetValue( 2u, body_sensor_delay ) );

				local_balance = dot_product( glob_ori, dof_rotation_axis ) + body_angvel_sensor_gain * dot_product( glob_angvel, dof_rotation_axis );
				Real org_lb = dot_product( global_balance, dof_rotation_axis );

				//std::cout << "gb=" << global_balance << " gori=" << glob_ori << " gav=" << glob_angvel << std::endl;
				//std::ofstream str( "d:/test.txt", std::ios_base::app );
				//str << global_balance[ 2 ] << "\t" << glob_ori[ 2 ] << std::endl;
			}
		}

		scone::Real MetaReflexDof::GetLocalBalance()
		{
			return local_balance;
		}

		void MetaReflexDof::StoreData( Storage< Real >::Frame& frame )
		{
		}

		void MetaReflexDof::SetDofRotationAxis()
		{
			dof_rotation_axis = target_dof.GetRotationAxis();
		}
	}
}
