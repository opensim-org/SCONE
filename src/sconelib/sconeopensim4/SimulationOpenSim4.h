/*
** SimulationOpenSim4.h
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
	class SCONE_OPENSIM_4_API SimulationOpenSim4 : public Simulation
	{
	public:
		SimulationOpenSim4( const PropNode& props );
		virtual ~SimulationOpenSim4() { };

		virtual void AdvanceSimulationTo( double time ) override;
	};
}
