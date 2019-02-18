/*
** SimulationOpenSim3.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SimulationOpenSim3.h"
#include "scone/core/Exception.h"
#include "ModelOpenSim3.h"

namespace scone
{
	SimulationOpenSim3::SimulationOpenSim3( const PropNode& props ) :
		Simulation( props )
	{
	}

	void SimulationOpenSim3::AdvanceSimulationTo( double time )
	{
		for ( auto iter = m_Models.begin(); iter != m_Models.end(); ++iter )
			dynamic_cast<ModelOpenSim3&>( **iter ).AdvanceSimulationTo( time );
	}
}
