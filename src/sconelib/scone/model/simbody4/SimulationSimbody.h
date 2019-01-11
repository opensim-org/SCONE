/*
** SimulationSimbody.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "sim_simbody.h"
#include "scone/model/Simulation.h"

namespace scone
{
	class SCONE_API SimulationSimbody : public Simulation
	{
	public:
		SimulationSimbody( const PropNode& props );
		virtual ~SimulationSimbody() { };

		virtual void AdvanceSimulationTo( double time ) override;
	};
}
