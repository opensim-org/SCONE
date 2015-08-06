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

		SensorDelayAdapter& Model::AcquireSensorDelayAdapter( Sensor& source )
		{
			auto it = std::find_if( m_SensorDelayAdapters.begin(), m_SensorDelayAdapters.end(),
				[&]( SensorDelayAdapterUP& a ) { return &a->GetSource() == &source; } );

			if ( it == m_SensorDelayAdapters.end() )
			{
				m_SensorDelayAdapters.push_back( SensorDelayAdapterUP( new SensorDelayAdapter( *this, source, 0.0 ) ) );
				return *m_SensorDelayAdapters.back();
			}
			else return **it;
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
