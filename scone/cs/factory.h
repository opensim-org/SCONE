#pragma once

#include "cs.h"
#include <memory>
#include "../sim/Simulation.h"
#include "../sim/simbody/Simulation_Simbody.h"
#include "../core/PropNode.h"

namespace scone
{
	namespace cs
	{
		namespace factory
		{
			template< typename T >
			std::shared_ptr< T > CreateSP( PropNode& props ) { return std::shared_ptr< T >( Create< T >( props ) ); }

			template< typename T >
			std::unique_ptr< T > CreateUP( PropNode& props ) { return std::unique_ptr< T >( Create< T >( props ) ); }

			template< typename T >
			std::vector< std::unique_ptr< T > > CreateVectorUP( PropNode& props )
			{ 
				std::vector< std::unique_ptr< T > > vec;
				for ( auto iter = props.Begin(); iter != props.End(); ++iter )
					vec.push_back( CreateUP< T >( props ) );
				return vec;
			}

			template< typename T >
			std::vector< std::shared_ptr< T > > CreateVectorSP( PropNode& props )
			{ 
				std::vector< std::shared_ptr< T > > vec;
				for ( auto iter = props.Begin(); iter != props.End(); ++iter )
					vec.push_back( CreateSP< T >( props ) );
				return vec;
			}

			// generic create template
			template< typename T >
			T* Create( PropNode& props ) { static_assert( false, "Create not defined for this type" ); }

			// create simulation
			template<> sim::Simulation* Create<>( PropNode& props );

			// create controller
			template<> cs::ParameterizableController* Create<>( PropNode& props );

			// create measure
			template<> Measure* Create<>( PropNode& props );
		}
	}
}
