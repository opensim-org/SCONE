#include "stdafx.h"
#include "FeedForwardController.h"
#include "../sim/sim.h"
#include "../sim/Controller.h"
#include "../sim/Model.h"
#include "../sim/Muscle.h"

#include <OpenSim/OpenSim.h>
#include "../core/Log.h"
#include <conio.h>

namespace scone
{
	namespace cs
	{
		FeedForwardController::FeedForwardController( const PropNode& props ) :
		Controller( props ),
		m_MuscleCount( 0 )
		{
			INIT_FROM_PROP( props, function_type, String("") );
			INIT_FROM_PROP( props, use_symmetric_actuators, true );
			INIT_FROM_PROP( props, control_points, 3u );
			INIT_FROM_PROP( props, control_point_time_delta, 0.3 );
			INIT_FROM_PROP( props, init_min, 0.0 );
			INIT_FROM_PROP( props, init_max, 1.0 );
			INIT_FROM_PROP( props, optimize_control_point_time, true );
		}

		void FeedForwardController::ProcessParameters( opt::ParamSet& par )
		{
			SCONE_ASSERT( m_MuscleCount > 0 );

			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
			{
				String str = m_MuscleNames[ idx ] + ".";
				for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
				{
					if ( optimize_control_point_time )
					{
						if ( cpidx > 0 )
						{
							double duration = par( str + GetStringF( "DT%d", cpidx - 1 ), control_point_time_delta, 0.1 * control_point_time_delta, 0.0, 60.0 );
							m_Functions[ idx ]->setX( cpidx, m_Functions[ idx ]->getX( cpidx - 1 ) + duration );
						}
						else m_Functions[ idx ]->setX( 0, 0.0 );
					}
					else m_Functions[ idx ]->setX( cpidx, cpidx * control_point_time_delta );
					m_Functions[ idx ]->setY( cpidx, par.GetMinMax( str + GetStringF( "Y%d", cpidx ), init_min, init_max, 0.0, 1.0 ) );
				}
			}
		}

		bool FeedForwardController::UpdateControls( sim::Model& model, double time )
		{
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

		void FeedForwardController::InitFromModel( sim::Model& model )
		{
			m_Functions.clear();
			m_MuscleNames.clear();

			m_MuscleCount = model.GetMuscles().size();
			size_t num_functions = use_symmetric_actuators ? m_MuscleCount / 2 : m_MuscleCount;
			for ( size_t idx = 0; idx < num_functions; ++idx )
			{
				m_Functions.push_back( FunctionUP( new OpenSim::PiecewiseLinearFunction() ) );
				for ( size_t cp = 0; cp < control_points; ++cp )
					m_Functions.back()->addPoint( 0, 0 );

				m_MuscleNames.push_back( model.GetMuscle( idx ).GetName() );
			}
		}
	}
}
