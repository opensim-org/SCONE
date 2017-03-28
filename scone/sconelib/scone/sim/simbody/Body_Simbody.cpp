#include "Body_Simbody.h"
#include "scone/core/Exception.h"

#include <OpenSim/OpenSim.h>
#include <OpenSim/Actuators/PointActuator.h>
#include "Model_Simbody.h"
#include "scone/core/Profiler.h"
#include "simbody_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	namespace sim
	{
		Body_Simbody::Body_Simbody( class Model_Simbody& model, OpenSim::Body& body ) :
		Body(),
		m_osBody( body ),
		m_Model( model ),
		m_ForceIndex( -1 ),
		m_LastNumDynamicsRealizations( -1 )
		{
			ConnectContactForce( body.getName() );
			SimTK::Vec3 com;
			m_osBody.getMassCenter( com );
			m_LocalComPos = ToVec3( com );
		}

		const String& Body_Simbody::GetName() const
		{
			return m_osBody.getName();
		}

		scone::Vec3 scone::sim::Body_Simbody::GetOriginPos() const
		{
			SCONE_PROFILE_FUNCTION;
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

			SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
			SimTK::Vec3 point;

			m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, zero, point );
			return ToVec3( point );
		}

		scone::Vec3 scone::sim::Body_Simbody::GetComPos() const
		{
			SCONE_PROFILE_FUNCTION;
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

			// TODO: OSIM: find what is the most efficient (compare to linvel)
			SimTK::Vec3 com;
			SimTK::Vec3 point;

			// TODO: validate this!
			m_osBody.getMassCenter( com );
			m_osBody.getModel().getSimbodyEngine().getPosition( m_Model.GetTkState(), m_osBody, com, point );
			return ToVec3( point );
		}

		scone::Vec3 Body_Simbody::GetLocalComPos() const
		{
			return m_LocalComPos;
		}

		scone::Quat scone::sim::Body_Simbody::GetOrientation() const
		{
			// TODO: cache this baby (after profiling), because sensors evaluate it for each channel
			auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			const auto& quat = mb.getBodyRotation( m_Model.GetTkState() ).convertRotationToQuaternion();
			Quat q1( quat[ 0 ], quat[ 1 ], quat[ 2 ], quat[ 3 ] );

			// OpenSim: can this be done more efficient?
			//double dir_cos[3][3];
			//m_osBody.getModel().getSimbodyEngine().getDirectionCosines( m_Model.GetTkState(), m_osBody, dir_cos );
			//double a1, a2, a3;
			//m_osBody.getModel().getSimbodyEngine().convertDirectionCosinesToAngles( dir_cos, &a1, &a2, &a3 );
			//Quat q2 = QuatFromEuler( Radian( a1 ), Radian( a2 ), Radian( a3 ) );

			return q1;
		}

		scone::Vec3 scone::sim::Body_Simbody::GetPosOfPointFixedOnBody(Vec3 point) const
		{
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Position );

			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.findStationLocationInGround( m_Model.GetTkState(), SimTK::Vec3(point.x, point.y, point.z)));
		
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetComVel() const
		{
			SCONE_PROFILE_FUNCTION;
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

			// TODO: OSIM: find what is the most efficient (compare to linvel)
			SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
			SimTK::Vec3 com;
			SimTK::Vec3 vel;

			// TODO: validate this!
			m_osBody.getMassCenter( com );
			m_osBody.getModel().getSimbodyEngine().getVelocity( m_Model.GetTkState(), m_osBody, com, vel );
			return ToVec3( vel );
		}

		scone::Vec3 scone::sim::Body_Simbody::GetOriginVel() const
		{
			SCONE_PROFILE_FUNCTION;

			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

			// TODO: OSIM: see if we can do this more efficient
			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.getBodyOriginVelocity( m_Model.GetTkState() ) );
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetAngVel() const
		{
			SCONE_PROFILE_FUNCTION;

			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

			// TODO: cache this baby (after profiling), because sensors evaluate it for each channel
			auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mb.getBodyAngularVelocity( m_Model.GetTkState() ) );
		}

		scone::Vec3 scone::sim::Body_Simbody::GetLinVelOfPointFixedOnBody(Vec3 point) const
		{
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Velocity );

			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.findStationVelocityInGround( m_Model.GetTkState(), SimTK::Vec3(point.x, point.y, point.z)));
		}


        scone::Vec3 scone::sim::Body_Simbody::GetComAcc() const
		{
			SCONE_PROFILE_FUNCTION;
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

			// TODO: OSIM: find what is the most efficient (compare to linvel)
			SimTK::Vec3 zero( 0.0, 0.0, 0.0 );
			SimTK::Vec3 com;
			SimTK::Vec3 acc;

			// TODO: validate this!
			m_osBody.getMassCenter( com );
			m_osBody.getModel().getSimbodyEngine().getAcceleration( m_Model.GetTkState(), m_osBody, com, acc );
			return ToVec3( acc );
		}

        scone::Vec3 scone::sim::Body_Simbody::GetOriginAcc() const
		{
			SCONE_PROFILE_FUNCTION;

			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

			// TODO: OSIM: see if we can do this more efficient
			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.getBodyOriginAcceleration( m_Model.GetTkState() ) );
		}
		
		scone::Vec3 scone::sim::Body_Simbody::GetAngAcc() const
		{
			SCONE_PROFILE_FUNCTION;

			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

			// TODO: cache this baby (after profiling), because sensors evaluate it for each channel
			auto& mb = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mb.getBodyAngularAcceleration( m_Model.GetTkState() ) );
		}

        scone::Vec3 scone::sim::Body_Simbody::GetLinAccOfPointFixedOnBody(Vec3 point) const
		{
			// TODO: see if we need to do this call to realize every time (maybe do it once before controls are updated)
			m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Acceleration );

			const SimTK::MobilizedBody& mob = m_osBody.getModel().getMultibodySystem().getMatterSubsystem().getMobilizedBody( m_osBody.getIndex() );
			return ToVec3( mob.findStationAccelerationInGround( m_Model.GetTkState(), SimTK::Vec3(point.x, point.y, point.z)));
		}

		Vec3 Body_Simbody::GetContactForce() const
		{
			SCONE_PROFILE_FUNCTION;
			if ( m_ForceIndex != -1 )
			{
				const auto& f = GetContactForceValues();
				return Vec3( -f[0], -f[1], -f[2] ); // entry 0-2 are forces applied to ground
			}
			else return Vec3::zero();
		}

		Vec3 Body_Simbody::GetContactMoment() const
		{
			if ( m_ForceIndex != -1 )
			{
				const auto& f = GetContactForceValues();
				return Vec3( -f[3], -f[4], -f[5] ); // entry 3-5 are moments applied to ground
			}
			else return Vec3::zero();
		}

		void Body_Simbody::ConnectContactForce( const String& force_name )
		{
			m_ForceIndex = m_osBody.getModel().getForceSet().getIndex( force_name, 0 );
			if ( m_ForceIndex != -1 )
			{
				auto labels = m_osBody.getModel().getForceSet().get( m_ForceIndex ).getRecordLabels();
				for ( int i = 0; i < labels.size(); ++i )
					m_ContactForceLabels.push_back( labels[ i ] );
				m_ContactForceValues.resize( m_ContactForceLabels.size() );
			}
		}

		const Model& Body_Simbody::GetModel() const 
		{
			return dynamic_cast< const Model& >( m_Model );
		}

		Model& Body_Simbody::GetModel()
		{
			return dynamic_cast< Model& >( m_Model );
		}

		std::vector< scone::String > Body_Simbody::GetDisplayGeomFileNames() const
		{
			std::vector< String > names;
			for ( int i = 0; i < m_osBody.getDisplayer()->getNumGeometryFiles(); ++i )
				names.push_back( m_osBody.getDisplayer()->getGeometryFileName( i ) );
			return names;
		}

		const std::vector< scone::Real >& Body_Simbody::GetContactForceValues() const
		{
			if ( m_ForceIndex != -1 )
			{
				m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
				int num_dyn = m_osBody.getModel().getMultibodySystem().getNumRealizationsOfThisStage( SimTK::Stage::Dynamics );

				if ( m_LastNumDynamicsRealizations != num_dyn )
				{
					// TODO: find out if this can be done less clumsy in OpenSim
					m_osBody.getModel().getMultibodySystem().realize( m_Model.GetTkState(), SimTK::Stage::Dynamics );
					OpenSim::Array<double> forces = m_osBody.getModel().getForceSet().get( m_ForceIndex ).getRecordValues( m_Model.GetTkState() );
					for ( int i = 0; i < forces.size(); ++i )
						m_ContactForceValues[i] = forces[i];
					m_LastNumDynamicsRealizations = num_dyn;
				}
			}
			return m_ContactForceValues;
		}

		void Body_Simbody::SetForce( const Vec3& f )
		{
			SetForceAtPoint( f, m_LocalComPos );
		}

		void Body_Simbody::SetForceAtPoint( const Vec3& force, const Vec3& point )
		{
			auto& cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() );
			cf.set_point( make_osim( point ) );
			cf.set_force( make_osim( force ) );
		}

		void Body_Simbody::SetTorque( const Vec3& torque )
		{
			auto& cf = m_Model.GetOsimBodyForce( m_osBody.getIndex() );
			cf.set_torque( make_osim( torque ) );
		}
	}
}
