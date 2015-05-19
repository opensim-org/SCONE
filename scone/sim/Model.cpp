#include "stdafx.h"
#include "Model.h"

#include "Body.h"
#include "Joint.h"
#include "Dof.h"
#include "Muscle.h"
#include "boost/foreach.hpp"
#include <algorithm>

#include "../core/Profiler.h"

using std::endl;

namespace scone
{
	namespace sim
	{
		Model::Model( const PropNode& props, opt::ParamSet& par ) :
		HasSignature( props ),
		m_ShouldTerminate( false )
		{
		}

		Model::~Model()
		{
		}

		Body& Model::FindBody( const String& name )
		{
			auto it = std::find_if( m_Bodies.begin(), m_Bodies.end(), [&]( BodyUP& body ) { return body->GetName() == name; } );
			if ( it == m_Bodies.end() )
				SCONE_THROW( "Could not find body: " + name );

			return **it;
		}

		std::ostream& Model::ToStream( std::ostream& str ) const
		{
			str << "Links:" << endl;
			str << GetRootLink().ToString();

			return str;
		}

		scone::Index Model::FindBodyIndex( const String& name )
		{
			auto it = std::find_if( m_Bodies.begin(), m_Bodies.end(), [&]( BodyUP& body ) { return body->GetName() == name; } );
			if ( it == m_Bodies.end() )
				return NoIndex;
			else return it - m_Bodies.begin();
		}

		SensorDelayAdapter& Model::AcquireSensorDelayAdapter( Sensor& source )
		{
			auto it = std::find_if( m_SensorDelayAdapters.begin(), m_SensorDelayAdapters.end(),
				[&]( SensorDelayAdapterUP& a ) { return &a->m_Source == &source; } );

			if ( it == m_SensorDelayAdapters.end() )
			{
				m_SensorDelayAdapters.push_back( SensorDelayAdapterUP( new SensorDelayAdapter( *this, m_SensorDelayStorage, source, 0.0 ) ) );
				return *m_SensorDelayAdapters.back();
			}
			else return **it;
		}

		void Model::UpdateSensorDelayAdapters()
		{
			SCONE_PROFILE_SCOPE;
			SCONE_CONDITIONAL_THROW( GetIntegrationStep() != GetPreviousIntegrationStep() + 1, "SensorDelayAdapters should only be updated at each new integration step" );
			SCONE_ASSERT( m_SensorDelayStorage.IsEmpty() || GetPreviousTime() == m_SensorDelayStorage.Back().GetTime() );

			// add a new frame and update
			m_SensorDelayStorage.AddFrame( GetTime() );
			BOOST_FOREACH( std::unique_ptr< SensorDelayAdapter >& sda, m_SensorDelayAdapters )
				sda->UpdateStorage();
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
	}
}
