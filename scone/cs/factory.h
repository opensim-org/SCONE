#pragma once

#include "cs.h"
#include <memory>
#include "../sim/Simulation.h"
#include "../sim/simbody/Simulation_Simbody.h"
#include "../core/PropNode.h"
#include "Objective.h"
#include "../opt/Optimizer.h"
#include "../core/Factory.h"

namespace scone
{
	//template<> CS_API Factory< cs::Objective >& GetFactory();

	namespace factory
	{
		// generic create template
		template< typename T >
		T* Create( const PropNode& props ) { static_assert( false, "Create not defined for this type" ); }

		template<> sim::Simulation* Create<>( const PropNode& props );
		template<> cs::ParameterizableController* Create<>( const PropNode& props );
		template<> cs::Measure* Create<>( const PropNode& props );
		template<> cs::Objective* Create<>( const PropNode& props );
		template<> opt::Optimizer* Create<>( const PropNode& props );
	}
}
