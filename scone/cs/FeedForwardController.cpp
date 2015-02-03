#include "stdafx.h"
#include "FeedForwardController.h"
#include "../sim/sim.h"
#include "../sim/Controller.h"
#include "../sim/Model.h"
#include "../sim/Muscle.h"
#include "../core/InitFromPropNode.h"

#include <OpenSim/OpenSim.h>
#include "../core/Log.h"
#include <conio.h>
#include <xutility>
#include "Tools.h"
#include "PieceWiseLinearFunction.h"
#include "Polynomial.h"
#include "PieceWiseConstantFunction.h"

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

		void FeedForwardController::Initialize( sim::Model& model )
		{
			m_Functions.clear();
			m_MuscleNames.clear();
			m_MuscleIndices.clear();
			m_MuscleModeWeights.clear();

			// setup muscle names and indices
			for ( size_t idx = 0; idx < model.GetMuscleCount(); ++idx )
			{
				if ( use_symmetric_actuators )
				{
					// lookup muscle
					String musName = GetSymmetricMuscleName( model.GetMuscle( idx ).GetName() );
					auto iter = std::find( m_MuscleNames.begin(), m_MuscleNames.end(), musName );
					if ( iter == m_MuscleNames.end() )
					{
						m_MuscleNames.push_back( musName );
						m_MuscleIndices.push_back( m_MuscleNames.size() - 1 );
					}
					else m_MuscleIndices.push_back( iter - m_MuscleNames.begin() );
				}
				else
				{
					m_MuscleNames.push_back( model.GetMuscle( idx ).GetName() );
					m_MuscleIndices.push_back( m_MuscleNames.size() - 1 );
				}
			}
		}

		void FeedForwardController::ProcessParameters( opt::ParamSet& par )
		{
			bool useModes = number_of_modes > 0;
			size_t num_functions = number_of_modes > 0 ? number_of_modes : m_MuscleNames.size();

			// process function parameters
			m_Functions.clear();
			for ( size_t idx = 0; idx < num_functions; ++idx )
			{
				String str = useModes ? GetStringF( "Mode%d.", idx ) : m_MuscleNames[ idx ] + ".";
				if ( function_type == "PieceWiseLinear" || function_type == "PieceWiseConstant" )
				{
					// TODO: fix this mess by creating a better parent class
					bool lin = function_type == "PieceWiseLinear";
					Function* pFunc;
					if (lin) pFunc = new PieceWiseLinearFunction( flat_extrapolation ); else pFunc = new PieceWiseConstantFunction();
					for ( size_t cpidx = 0; cpidx < control_points; ++cpidx )
					{
						Real xVal = 0.0;
						if ( optimize_control_point_time )
						{
							if ( cpidx > 0 )
							{
								double duration = par( str + GetStringF( "DT%d", cpidx - 1 ), control_point_time_delta, 0.1 * control_point_time_delta, 0.0, 60.0 );
								xVal = lin ? dynamic_cast<PieceWiseLinearFunction*>(pFunc)->GetOsFunc().getX( cpidx - 1 ) + duration : dynamic_cast<PieceWiseConstantFunction*>(pFunc)->GetOsFunc().getX( cpidx - 1 ) + duration;
							}
						}
						else xVal = cpidx * control_point_time_delta;

						// Y value
						Real yVal = par.GetMinMax( str + GetStringF( "Y%d", cpidx ), init_min, init_max, useModes ? -1.0 : 0.0, 1.0 );
						if ( lin ) dynamic_cast<PieceWiseLinearFunction*>(pFunc)->GetOsFunc().addPoint( xVal, yVal );
						else dynamic_cast<PieceWiseConstantFunction*>(pFunc)->GetOsFunc().addPoint( xVal, yVal );
					}
					m_Functions.push_back( FunctionUP( pFunc ) );
				}
				else if ( function_type == "Polynomial" )
				{
					Polynomial* pFunc = new Polynomial( control_points );
					for ( size_t i = 0; i < pFunc->GetCoefficientCount(); ++i )
					{
						if ( i == 0 )
							pFunc->SetCoefficient( i, par.GetMinMax( str + GetStringF( "Coeff%d", i ), init_min, init_max, 0.0, 1.0 ) );
						else pFunc->SetCoefficient( i, par.GetMinMax( str + GetStringF( "Coeff%d", i ), init_mode_weight_min, init_mode_weight_max, -1.0, 1.0 ) );
					}
					m_Functions.push_back( FunctionUP( pFunc ) );
				}
				else SCONE_THROW( "Unknown function type: " + function_type );
			}

			// process muscle mode weights
			m_MuscleModeWeights.resize( m_MuscleNames.size(), std::vector< double >( number_of_modes, 0.0 ) );
			for ( size_t mus = 0; mus < m_MuscleNames.size(); ++mus )
			{
				for ( size_t mode = 0; mode < number_of_modes; ++mode )
					m_MuscleModeWeights[ mus ][ mode ] = par.GetMinMax( m_MuscleNames[ mus ] + GetStringF( ".Mode%d", mode ), init_mode_weight_min, init_mode_weight_max, -1.0, 1.0 );
			}
		}

		void FeedForwardController::UpdateControls( sim::Model& model, double time )
		{
			// evaluate functions
			std::vector< double > funcresults( m_Functions.size() );
			SimTK::Vector xval( 1 );
			for ( size_t idx = 0; idx < m_Functions.size(); ++idx )
				funcresults[ idx ] = m_Functions[ idx ]->GetValue( time );

			if ( number_of_modes > 0 )
			{
				// apply result of each mode to all muscles
				for ( size_t mode = 0; mode < number_of_modes; ++mode )
				{
					for ( size_t idx = 0; idx < m_MuscleIndices.size(); ++idx )
						model.GetMuscle( idx ).AddControlValue( funcresults[ mode ] * m_MuscleModeWeights[ m_MuscleIndices[ idx ] ][ mode ] );
				}
			}
			else
			{
				// apply results directly to control value
				for ( size_t idx = 0; idx < m_MuscleIndices.size(); ++idx )
					model.GetMuscle( idx ).AddControlValue( funcresults[ m_MuscleIndices[ idx ] ] );
			}
		}
	}
}
