#include "stdafx.h"
#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Dof.h"
#include "Muscle.h"
#include "boost/foreach.hpp"
#include <algorithm>

#include "../core/Profiler.h"
#include "SensorDelayAdapter.h"
#include "../core/InitFromPropNode.h"
#include "Factories.h"

using std::endl;

namespace scone
{
	namespace sim
	{
		Model::Model( const PropNode& props, opt::ParamSet& par ) :
		HasSignature( props ),
		m_ShouldTerminate( false ),
		custom_properties( props.HasKey( "custom_properties" ) ? props.GetChild( "custom_properties" ) : PropNode::EMPTY_PROP_NODE )
		{
			INIT_PROPERTY( props, sensor_delay_scaling_factor, 1.0 );
		}

		Model::~Model()
		{
		}

		std::ostream& Model::ToStream( std::ostream& str ) const
		{
			str << "Links:" << endl;
			str << GetRootLink().ToString();

			return str;
		}

		Sensor& Model::AcquireSensor( const PropNode& pn, opt::ParamSet& par, const sim::Area& area )
		{
			// create the sensor first, so we can use its name to find it
			SensorUP sensor = sim::CreateSensor( pn, par, *this, area );

			// see if there's a sensor with the same name (should be unique)
			auto it = std::find_if( m_Sensors.begin(), m_Sensors.end(), [&]( SensorUP& s ) { return s->GetName() == sensor->GetName(); } );

			if ( it == m_Sensors.end() )
			{
				// add the new sensor and return it
				m_Sensors.push_back( std::move( sensor ) );
				return *m_Sensors.back(); // return newly added sensor
			}
			else return **it; // return found element (sensor gets deleted)
		}

		SensorDelayAdapter& Model::AcquireSensorDelayAdapter( Sensor& source )
		{
			auto it = std::find_if( m_SensorDelayAdapters.begin(), m_SensorDelayAdapters.end(),
				[&]( SensorDelayAdapterUP& a ) { return &a->GetInputSensor() == &source; } );

			if ( it == m_SensorDelayAdapters.end() )
			{
				m_SensorDelayAdapters.push_back( SensorDelayAdapterUP( new SensorDelayAdapter( *this, source, 0.0 ) ) );
				return *m_SensorDelayAdapters.back();
			}
			else return **it;
		}

		SensorDelayAdapter& Model::AcquireDelayedSensor( const PropNode& pn, opt::ParamSet& par, const sim::Area& area )
		{
			// acquire sensor first
			return AcquireSensorDelayAdapter( AcquireSensor( pn, par, area ) );
		}

		void Model::UpdateSensorDelayAdapters()
		{
			SCONE_PROFILE_SCOPE;
			//SCONE_THROW_IF( GetIntegrationStep() != GetPreviousIntegrationStep() + 1, "SensorDelayAdapters should only be updated at each new integration step" );
			SCONE_ASSERT( m_SensorDelayStorage.IsEmpty() || GetPreviousTime() == m_SensorDelayStorage.Back().GetTime() );

			// add a new frame and update
			m_SensorDelayStorage.AddFrame( GetTime() );
			BOOST_FOREACH( std::unique_ptr< SensorDelayAdapter >& sda, m_SensorDelayAdapters )
				sda->UpdateStorage();

			//log::TraceF( "Updated Sensor Delays for Int=%03d time=%.6f prev_time=%.6f", GetIntegrationStep(), GetTime(), GetPreviousTime() );
		}

		void Model::UpdateControlValues()
		{
			SCONE_PROFILE_SCOPE;

			// reset actuator values
			// TODO: not only muscles!
			BOOST_FOREACH( MuscleUP& mus, GetMuscles() )
				mus->ResetControlValue();

			// update all controllers
			bool terminate = false;
			BOOST_FOREACH( ControllerUP& con, GetControllers() )
				terminate |= con->UpdateControls( *this, GetTime() ) == Controller::RequestTermination;

			//log::TraceF( "Controls updated for Int=%03d time=%.6f", GetIntegrationStep(), GetTime() );

			if ( terminate )
				SetTerminationRequest();
		}

		void Model::UpdateAnalyses()
		{
			SCONE_PROFILE_SCOPE;

			bool terminate = false;
			BOOST_FOREACH( ControllerUP& con, GetControllers() )
				terminate |= con->UpdateAnalysis( *this, GetTime() ) == sim::Controller::RequestTermination;

			if ( terminate )
				SetTerminationRequest();
		}

		const Link& Model::FindLink( const String& body_name )
		{
			const Link* link = GetRootLink().FindLink( body_name );
			SCONE_THROW_IF( link == nullptr, "Could not find link " + body_name );
			return *link;
		}

		scone::Real Model::GetTotalContactForce() const
		{
			Real force = 0.0;
			BOOST_FOREACH( const LegUP& leg, GetLegs() )
				force += leg->GetContactForce().GetLength();
			return force;
		}
	}
}
