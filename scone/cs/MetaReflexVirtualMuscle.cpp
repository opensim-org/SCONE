#include "stdafx.h"
#include "MetaReflexVirtualMuscle.h"
#include "../sim/Model.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Muscle.h"

namespace scone
{
	namespace cs
	{
		MetaReflexVirtualMuscle::MetaReflexVirtualMuscle( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& area ) :
		local_balance( 0 ),
		body_angvel_sensor_gain( model.balance_sensor_ori_vel_gain ),
		body_ori_sensor( nullptr ),
		body_angvel_sensor( nullptr )
		{
			// TODO: remove once a proper factory is used
			SCONE_ASSERT( props.GetStr( "type" ) == "VirtualMuscleReflex" );

			// parse target
			std::stringstream str( props.GetStr( "target" ) );
			while ( str.good() )
			{
				String dofname;
				Real weight;
				str >> dofname >> weight;
				if ( !str.fail() )
				{
					if ( HasElementWithName( model.GetDofs(), dofname ) ) // see if the dof is unsided
						dof_infos.push_back( DofInfo{ *FindByName( model.GetDofs(), dofname ), weight } );
					else // try to add a sided version of the dof
						dof_infos.push_back( DofInfo{ *FindByName( model.GetDofs(), dofname + GetSideName( area.side ) ), weight } );
					name += dofname + ( weight > 0 ? '+' : '-' );
				}
				else break;
			}

			opt::ScopedParamSetPrefixer prefixer( par, name + "." );

			// init reflex parameters
			mrpar.Init( props, par, model );

			// init balance parameters
			if ( model.GetCustomProp( "meta_reflex_control.use_balance", true ) && props.HasKey( "Balance" ) )
			{
				opt::ScopedParamSetPrefixer pre2( par, "B." );
				auto& balprops = props.GetChild( "Balance" );
				auto& body = FindByName( model.GetBodies(), balprops.GetStr( "body" ) );

				// create sensors
				body_ori_sensor = &model.AcquireDelayedSensor< sim::BodyOriSensor >( *body );
				body_angvel_sensor = &model.AcquireDelayedSensor< sim::BodyAngVelSensor >( *body );

				bal_mrpar.Init( balprops, par, model );

				INIT_PARAM_NAMED( balprops, par, body_angvel_sensor_gain, "KAV", model.balance_sensor_ori_vel_gain );
			}

			// TODO: move to muscle
			INIT_PROPERTY_REQUIRED( props, delay );
		}

		scone::Real MetaReflexVirtualMuscle::GetSimilarity( const sim::Muscle& mus, Real tot_abs_moment_arm )
		{
			Real similarity = 0;
			for ( auto& di : dof_infos )
			{
				if ( mus.HasMomentArm( di.dof ) )
					similarity += di.w * ( mus.GetMomentArm( di.dof ) / tot_abs_moment_arm );
			}
			return similarity;
		}
	}
}
