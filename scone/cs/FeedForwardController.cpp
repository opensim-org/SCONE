#include "stdafx.h"
#include "FeedForwardController.h"
#include "../sim/sim.h"
#include "../sim/Controller.h"
#include "../sim/Model.h"
#include "../sim/Muscle.h"
#include "../core/InitFromPropNode.h"

#include "../core/Log.h"
#include <conio.h>
#include <xutility>
#include "Tools.h"
#include "PieceWiseLinearFunction.h"
#include "Polynomial.h"
#include "PieceWiseConstantFunction.h"

#include <boost/foreach.hpp>
#include <OpenSim/Common/PiecewiseLinearFunction.h>
#include <OpenSim/Common/PiecewiseConstantFunction.h>

namespace scone
{
	namespace cs
	{
		FeedForwardController::FeedForwardController( const PropNode& props ) :
		Controller( props )
		{
			INIT_FROM_PROP( props, function_type, String("") );
			INIT_FROM_PROP( props, use_symmetric_actuators, true );
			INIT_FROM_PROP( props, control_points, 3u );
			INIT_FROM_PROP( props, control_point_time_delta, 0.3 );
			INIT_FROM_PROP( props, init_min, 0.0 );
			INIT_FROM_PROP( props, init_max, 1.0 );
			INIT_FROM_PROP( props, init_mode_weight_min, -1.0 );
			INIT_FROM_PROP( props, init_mode_weight_max, 1.0 );
			INIT_FROM_PROP( props, optimize_control_point_time, true );
			INIT_FROM_PROP( props, flat_extrapolation, false );
			INIT_FROM_PROP( props, number_of_modes, 0u );
		}

		void FeedForwardController::Initialize( sim::Model& model, opt::ParamSet& par, const PropNode& props )
		{
			m_Functions.clear();
			m_ActInfos.clear();

			// setup actuator info
			for ( size_t idx = 0; idx < model.GetMuscleCount(); ++idx )
			{
				ActInfo ai;
				ai.full_name = model.GetMuscle( idx ).GetName();
				ExtractNameAndSide( model.GetMuscle( idx ).GetName(), ai.name, ai.side );
				m_ActInfos.push_back( ai );
			}

			// create functions
			if ( UseModes() )
			{
				// create mode functions
				for ( size_t idx = 0; idx < number_of_modes; ++idx )
					m_Functions.push_back( FunctionUP( CreateFunction( par, GetStringF( "Mode%d.", idx ) ) ) );
			}

			BOOST_FOREACH( ActInfo& ai, m_ActInfos )
			{
				if ( use_symmetric_actuators )
				{
					// check if we've already processed a mirrored version of this ActInfo
					auto it = std::find_if( m_ActInfos.begin(), m_ActInfos.end(), [&]( ActInfo& oai ) { return ai.name == oai.name; } );
					if ( it->function_idx != NO_INDEX || !it->mode_weights.empty() )
					{
						ai.function_idx = it->function_idx;
						ai.mode_weights = it->mode_weights;
						continue;
					}
				}

				if ( UseModes() )
				{
					// set mode weights
					ai.mode_weights.resize( number_of_modes );
					String prefix = use_symmetric_actuators ? ai.name : ai.full_name;
					for ( size_t mode = 0; mode < number_of_modes; ++mode )
						ai.mode_weights[ mode ] = par.GetMinMax( prefix + GetStringF( ".Mode%d", mode ), init_mode_weight_min, init_mode_weight_max, -1.0, 1.0 );
				}
				else
				{
					// create a new function
					String prefix = use_symmetric_actuators ? ai.name : ai.full_name;
					m_Functions.push_back( FunctionUP( CreateFunction( par, prefix + "." ) ) );
					ai.function_idx = m_Functions.size() - 1;
				}
			}
		}

		void FeedForwardController::UpdateControls( sim::Model& model, double time )
		{
			// evaluate functions
			std::vector< double > funcresults( m_Functions.size() );
			SimTK::Vector xval( 1 );
			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
				funcresults[ idx ] = m_Functions[ idx ]->GetValue( time );

			if ( UseModes() )
			{
				// apply result of each mode to all muscles
				for ( size_t idx = 0; idx < m_ActInfos.size(); ++idx )
				{
					Real val = 0.0;
					for ( size_t mode = 0; mode < number_of_modes; ++mode )
						val += funcresults[ mode ] * m_ActInfos[ idx ].mode_weights[ mode ];

					// add control value
					model.GetMuscle( idx ).AddControlValue( val );

				}
			}
			else
			{
				// apply results directly to control value
				for ( size_t idx = 0; idx < m_ActInfos.size(); ++idx )
					model.GetMuscle( idx ).AddControlValue( funcresults[ m_ActInfos[ idx ].function_idx ] );
			}
		}

		Function* FeedForwardController::CreateFunction( opt::ParamSet &par, const String& prefix )
		{
			if ( function_type == "PieceWiseLinear" || function_type == "PieceWiseConstant" )
			{
				// TODO: fix this mess by creating a PieceWiseFunction parent class
				Function* pFunc = nullptr;
				bool lin = function_type == "PieceWiseLinear";
				if (lin) pFunc = new PieceWiseLinearFunction( flat_extrapolation );
				else pFunc = new PieceWiseConstantFunction();

				for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
				{
					Real xVal = 0.0;
					if ( optimize_control_point_time )
					{
						if ( cpidx > 0 )
						{
							double duration = par( prefix + GetStringF( "DT%d", cpidx - 1 ), control_point_time_delta, 0.1 * control_point_time_delta, 0.0, 60.0 );
							xVal = lin ? dynamic_cast<PieceWiseLinearFunction*>(pFunc)->GetOsFunc().getX( cpidx - 1 ) + duration : dynamic_cast<PieceWiseConstantFunction*>(pFunc)->GetOsFunc().getX( cpidx - 1 ) + duration;
						}
					}
					else xVal = cpidx * control_point_time_delta;

					// Y value
					Real yVal = par.GetMinMax( prefix + GetStringF( "Y%d", cpidx ), init_min, init_max, UseModes() ? -1.0 : 0.0, 1.0 );
					if ( lin ) dynamic_cast<PieceWiseLinearFunction*>(pFunc)->GetOsFunc().addPoint( xVal, yVal );
					else dynamic_cast<PieceWiseConstantFunction*>(pFunc)->GetOsFunc().addPoint( xVal, yVal );
				}
				return pFunc;
			}
			else if ( function_type == "Polynomial" )
			{
				Polynomial* pFunc = new Polynomial( control_points );
				for ( size_t i = 0; i < pFunc->GetCoefficientCount(); ++i )
				{
					if ( i == 0 )
						pFunc->SetCoefficient( i, par.GetMinMax( prefix + GetStringF( "Coeff%d", i ), init_min, init_max, 0.0, 1.0 ) );
					else pFunc->SetCoefficient( i, par.GetMinMax( prefix + GetStringF( "Coeff%d", i ), init_mode_weight_min, init_mode_weight_max, -1.0, 1.0 ) );
				}
				return pFunc;
			}
			else SCONE_THROW( "Unknown function type: " + function_type );
		}
	}
}
