#include "Sensors.h"
#include "Model.h"
#include "Muscle.h"
#include "Area.h"
#include "Body.h"
#include "Dof.h"
#include "scone/core/string_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	namespace sim
	{
		MuscleSensor::MuscleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
			Sensor( pn, par, model, target_area ),
			m_Muscle( *FindByName( model.GetMuscles(), pn.GetStr( "muscle" ) ) )
		{}

		scone::Real MuscleForceSensor::GetValue() const
		{
			return m_Muscle.GetNormalizedForce();
		}

		scone::String MuscleForceSensor::GetName() const
		{
			return m_Muscle.GetName() + ".F";
		}

		scone::Real MuscleLengthSensor::GetValue() const
		{
			return m_Muscle.GetNormalizedFiberLength();
		}

		scone::String MuscleLengthSensor::GetName() const
		{
			return m_Muscle.GetName() + ".L";
		}

		scone::Real MuscleVelocitySensor::GetValue() const
		{
			return m_Muscle.GetNormalizedFiberVelocity();
		}

		scone::String MuscleVelocitySensor::GetName() const
		{
			return m_Muscle.GetName() + ".V";
		}

		scone::Real MuscleSpindleSensor::GetValue() const
		{
			// derived from [Prochazka1999], but normalized to unit length
			return 0.105 * sqrt( std::max( 0.0, m_Muscle.GetNormalizedFiberVelocity() ) ) + m_Muscle.GetNormalizedFiberLength();
		}

		scone::String MuscleSpindleSensor::GetName() const
		{
			return m_Muscle.GetName() + ".MS";
		}

		DofSensor::DofSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
			Sensor( pn, par, model, target_area ),
			m_Dof( *FindByName( model.GetDofs(), pn.GetStr( "dof" ) ) ),
			m_pRootDof( pn.HasKey( "root_dof" ) ? FindByName( model.GetDofs(), pn.GetStr( "root_dof" ) ).get() : nullptr )
		{}

		scone::Real DofPositionSensor::GetValue() const
		{
			// TODO: get rid of this if statement and use a "constant" Dof?
			if ( m_pRootDof )
				return m_pRootDof->GetPos() + m_Dof.GetPos();
			else return m_Dof.GetPos();
		}

		scone::String DofPositionSensor::GetName() const
		{
			return m_Dof.GetName() + ".P";
		}

		scone::Real DofVelocitySensor::GetValue() const
		{
			// TODO: get rid of this if statement and use a "constant" Dof?
			if ( m_pRootDof )
				return m_pRootDof->GetVel() + m_Dof.GetVel();
			else return m_Dof.GetVel();
		}

		scone::String DofVelocitySensor::GetName() const
		{
			return m_Dof.GetName() + ".V";
		}

		LegLoadSensor::LegLoadSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
			Sensor( pn, par, model, target_area ),
			m_Leg( *FindBySide( model.GetLegs(), target_area.side ) )
		{}

		scone::Real LegLoadSensor::GetValue() const
		{
			return m_Leg.GetLoad();
		}

		scone::String LegLoadSensor::GetName() const
		{
			return m_Leg.GetName() + ".L";
		}

		static const char* g_PelvisNames[] = { "pelvis_tilt", "pelvis_list", "pelvis_rotation" };
		static const char* g_LumbarNames[] = { "lumbar_extension", "lumbar_bending", "lumbar_rotation" };
		static const char* g_PlaneNames[] = { "Sagittal", "Coronal", "Transverse" };

		// TODO: get rid of hard-coded dof names
		OrientationSensor::OrientationSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Plane( Invalid ),
		m_Pelvis( nullptr ), 
		m_Lumbar( nullptr )
		{
			// init plane
			m_Plane = (Plane)pn.GetInt( "plane", -1 );
			if ( m_Plane == -1 )
			{
				// try name (TODO: lower case comparison)
				for ( int i = 0; i < 3; ++i )
					if ( pn.GetStr( "plane" ) == g_PlaneNames[ i ] )
						m_Plane = Plane( i );
			}
			SCONE_ASSERT_MSG( m_Plane >= 0 && m_Plane < 3, "Invalid plane: " + quoted( pn.GetStr( "plane" ) ) );

			// init Dofs (if they exist)
            // first check if pelvis exists, if not don't add a sensor. then add lumbar if it exists.
			if ( HasElementWithName( model.GetDofs(), g_PelvisNames[ m_Plane ] ) )
			{
				m_Pelvis = FindByName( model.GetDofs(), g_PelvisNames[ m_Plane ] ).get();
                if ( HasElementWithName(model.GetDofs(), g_LumbarNames[ m_Plane ] ) ) 
                    m_Lumbar = FindByName( model.GetDofs(), g_LumbarNames[m_Plane] ).get();
			}
			else log::Warning( "Could not find Dof for balance sensor: " + String( g_PelvisNames[ m_Plane ] ) );

			opt::ScopedParamSetPrefixer prefixer( par, GetName() );
			INIT_PARAM_NAMED( pn, par, m_PosGain, "kp", 1.0 );
			INIT_PARAM_NAMED( pn, par, m_VelGain, "kd", 0.0 );
		}

		OrientationSensor::OrientationSensor( Model& model, Plane plane, Real posgain, Real velgain ) :
		m_Plane( plane ),
		m_PosGain( posgain ),
		m_VelGain( velgain ),
		m_Pelvis( nullptr ),
		m_Lumbar( nullptr )
		{
            // init Dofs (if they exist)
            // first check if pelvis exists, if not don't add a sensor. then add lumbar if it exists.
            if (HasElementWithName(model.GetDofs(), g_PelvisNames[m_Plane]))
            {
                m_Pelvis = FindByName(model.GetDofs(), g_PelvisNames[m_Plane]).get();
                if (HasElementWithName(model.GetDofs(), g_LumbarNames[m_Plane]))
                    m_Lumbar = FindByName(model.GetDofs(), g_LumbarNames[m_Plane]).get();
            }
		}

        scone::Real OrientationSensor::GetValue() const
        {
            if (m_Pelvis) {
                if (m_Lumbar)
                    return m_PosGain * (m_Pelvis->GetPos() + m_Lumbar->GetPos()) + m_VelGain * (m_Pelvis->GetVel() + m_Lumbar->GetVel());
                else
                    return m_PosGain * (m_Pelvis->GetPos()) + m_VelGain * (m_Pelvis->GetVel());
            }
			else return 0.0;
		}

		scone::String OrientationSensor::GetName() const
		{
			return String( "Ori." ) + g_PlaneNames[ m_Plane ];
		}

		BodySensor::BodySensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) :
		Sensor( pn, par, model, target_area ),
		m_Body( *FindByName( model.GetBodies(), pn.GetStr( "body" ) ) )
		{}

		const char* g_BodyChannelNames[] = { "X", "Y", "Z" };

		scone::Real BodyOriSensor::GetValue( Index idx ) const
		{
			return flut::math::make_rotation_vector( m_Body.GetOri() )[ idx ];
		}

		scone::String BodyOriSensor::GetName() const
		{
			return m_Body.GetName() + ".Ori";
		}

		scone::Real BodyAngVelSensor::GetValue( Index idx ) const
		{
			return m_Body.GetAngVel()[ idx ];
		}

		scone::String BodyAngVelSensor::GetName() const
		{
			return m_Body.GetName() + ".AngVel";
		}
	}
}
