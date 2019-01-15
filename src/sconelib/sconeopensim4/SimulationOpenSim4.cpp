/*
** SimulationOpenSim4.cpp
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "SimulationOpenSim4.h"
#include "scone/core/Exception.h"
#include "ModelOpenSim4.h"

namespace scone
{
	SimulationOpenSim4::SimulationOpenSim4( const PropNode& props ) :
		Simulation( props )
	{
	}

	void SimulationOpenSim4::AdvanceSimulationTo( double time )
	{
		for ( auto iter = m_Models.begin(); iter != m_Models.end(); ++iter )
			dynamic_cast<ModelOpenSim4&>( **iter ).AdvanceSimulationTo( time );
	}
}
