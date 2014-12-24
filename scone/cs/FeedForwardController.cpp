#include "stdafx.h"
#include "FeedForwardController.h"
#include "../sim/sim.h"
#include "../sim/Controller.h"
#include "../sim/Model.h"
#include "../sim/Muscle.h"

#include <OpenSim/OpenSim.h>
#include "../core/Log.h"

namespace scone
{
	namespace cs
	{
		void FeedForwardController::ProcessProperties( const PropNode& props )
		{
			Controller::ProcessProperties( props );

			INIT_FROM_PROP( props, function_type, String("") );
			INIT_FROM_PROP( props, use_symmetric_actuators, true );
			INIT_FROM_PROP( props, control_points, 3u );
			INIT_FROM_PROP( props, initial_time_separation, 0.3 );
			INIT_FROM_PROP( props, init_mean, 0.5 );
			INIT_FROM_PROP( props, init_std, 0.25 );
			INIT_FROM_PROP( props, use_fixed_time, true );
		}

		void FeedForwardController::ProcessParameters( opt::ParamSet& par )
		{
			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
			{
				String str = m_MuscleNames[ idx ] + ".";
				for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
				{
					m_Functions[ idx ]->setX( cpidx, par( str + ToString( cpidx ) + ".X", init_mean, init_std, 0.0, 1.0 ) );
					m_Functions[ idx ]->setY( cpidx, par( str + ToString( cpidx ) + ".Y", init_mean, init_std, 0.0, 1.0 ) );
				}
			}
		}

		bool FeedForwardController::UpdateControls( sim::Model& model, double time )
		{
			SCONE_LOG( model.GetMuscleCount() );
			SCONE_LOG( m_MuscleCount );
			SCONE_ASSERT( model.GetMuscleCount() == m_MuscleCount );

			SimTK::Vector xval( 1 );
			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
			{
				xval[ 0 ] = time;
				double result = m_Functions[ idx ]->calcValue( xval );
				model.GetMuscle( idx ).AddControlValue( result );
				if ( use_symmetric_actuators )
					model.GetMuscle( m_Functions.size() + idx ).AddControlValue( result );
			}

			return true;
		}

		void FeedForwardController::ConnectModel( sim::Model& model )
		{
			SCONE_ASSERT( m_MuscleCount == 0 );

			m_MuscleCount = model.GetMuscles().size();
			size_t num_functions = use_symmetric_actuators ? m_MuscleCount / 2 : m_MuscleCount;
			for ( size_t idx = 0; idx < num_functions; ++idx )
			{
				std::vector< double > zeros( control_points, 0.0 ); // dummy vector for initialization
				m_Functions.push_back( FunctionUP( new OpenSim::PiecewiseLinearFunction( (int)control_points, &zeros[0], &zeros[0] ) ) );
				m_MuscleNames.push_back( model.GetMuscle( idx ).GetName() );
			}
		}

	}
}
