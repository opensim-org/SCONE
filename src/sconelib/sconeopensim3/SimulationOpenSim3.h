/*
** SimulationOpenSim3.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/Simulation.h"

namespace scone
{
	class SCONE_OPENSIM_3_API SimulationOpenSim3 : public Simulation
	{
	public:
		SimulationOpenSim3( const PropNode& props );
		virtual ~SimulationOpenSim3() { };

		virtual void AdvanceSimulationTo( double time ) override;
	};
}
