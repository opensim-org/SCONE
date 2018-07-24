#include "PointMeasure.h"
#include "scone/model/Model.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"
#include "scone/core/math.h"

namespace scone
{
	PointMeasure::PointMeasure( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Measure( props, par, model, area ),
		m_pTargetBody( nullptr )
	{
		INIT_PROPERTY_REQUIRED( props, body );
		INIT_PROPERTY( props, offset, Vec3::zero() );
		INIT_PROPERTY_REQUIRED( props, axes_to_measure );
		INIT_PROPERTY( props, relative_to_model_com, false );
		INIT_PROPERTY( props, squared_range_penalty, 0.0 );
		INIT_PROPERTY( props, abs_range_penalty, 0.0 );
		INIT_PROPERTY( props, squared_velocity_range_penalty, 0.0 );
		INIT_PROPERTY( props, abs_velocity_range_penalty, 0.0 );
		INIT_PROPERTY( props, squared_acceleration_range_penalty, 0.0 );
		INIT_PROPERTY( props, abs_acceleration_range_penalty, 0.0 );

		// find target body
		if ( !body.empty() )
			m_pTargetBody = FindByName( model.GetBodies(), body ).get();
		else SCONE_THROW( "cannot find body name '" + body + "'" );

		// initialize range
		range.min = Real( props.get< Real >( "pos_min", 0.0 ) );
		range.max = Real( props.get< Real >( "pos_max", 0.0 ) );
		vel_range.min = Real( props.get< Real >( "vel_min", 0.0 ) );
		vel_range.max = Real( props.get< Real >( "vel_max", 0.0 ) );
		acc_range.min = Real( props.get< Real >( "acc_min", 0.0 ) );
		acc_range.max = Real( props.get< Real >( "acc_max", 0.0 ) );

		// make axes_to_measure all 1's and 0's
		for ( int i = 0; i < 2; ++i ) {
			axes_to_measure[ i ] = ( axes_to_measure[ i ] > 0 ) ? 1 : 0;
		}
	}

	bool PointMeasure::UpdateMeasure( const Model& model, double timestamp )
	{
		SCONE_PROFILE_FUNCTION;

		if ( squared_range_penalty > 0.0 || abs_range_penalty > 0.0 )
		{
			Vec3 vec_pos;
			if ( !relative_to_model_com ) vec_pos = m_pTargetBody->GetPosOfPointFixedOnBody( offset );
			else vec_pos = m_pTargetBody->GetPosOfPointFixedOnBody( offset ) - model.GetComPos();

			for ( int i = 0; i < 2; ++i ) {
				vec_pos[ i ] = vec_pos[ i ] * axes_to_measure[ i ];
			}
			double range_violation = range.GetRangeViolation( vec_pos.length() );
			double rps = squared_range_penalty * GetSquared( range_violation );
			double rpa = abs_range_penalty * std::abs( range_violation );
			penalty.AddSample( timestamp, rps + rpa );
		}

		if ( squared_velocity_range_penalty > 0 || abs_velocity_range_penalty > 0 )
		{
			Vec3 vec_vel;
			if ( !relative_to_model_com ) vec_vel = m_pTargetBody->GetLinVelOfPointFixedOnBody( offset );
			else vec_vel = m_pTargetBody->GetLinVelOfPointFixedOnBody( offset ) - model.GetComVel();

			for ( int i = 0; i < 2; ++i ) {
				vec_vel[ i ] = vec_vel[ i ] * axes_to_measure[ i ];
			}
			double range_violation = vel_range.GetRangeViolation( vec_vel.length() );
			double vrps = squared_velocity_range_penalty * GetSquared( range_violation );
			double vrpa = abs_velocity_range_penalty * std::abs( range_violation );
			penalty.AddSample( timestamp, vrps + vrpa );
		}

		if ( squared_acceleration_range_penalty > 0 || abs_acceleration_range_penalty > 0 )
		{
			Vec3 vec_acc;
			if ( !relative_to_model_com ) vec_acc = m_pTargetBody->GetLinAccOfPointFixedOnBody( offset );
			else vec_acc = m_pTargetBody->GetLinAccOfPointFixedOnBody( offset ) - model.GetComAcc();

			for ( int i = 0; i < 2; ++i ) {
				vec_acc[ i ] = vec_acc[ i ] * axes_to_measure[ i ];
			}
			double range_violation = acc_range.GetRangeViolation( vec_acc.length() );
			double arps = squared_acceleration_range_penalty * GetSquared( range_violation );
			double arpa = abs_acceleration_range_penalty * std::abs( range_violation );
			penalty.AddSample( timestamp, arps + arpa );
		}

		return false;
	}

	double PointMeasure::GetResult( Model& model )
	{
		return penalty.GetAverage();
	}

	scone::String PointMeasure::GetClassSignature() const
	{
		return "";
	}

	void PointMeasure::StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const
	{
		frame[ m_pTargetBody->GetName() + ".point_penalty" ] = penalty.GetLatest();
	}
}
