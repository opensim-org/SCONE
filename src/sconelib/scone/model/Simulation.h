/*
** Simulation.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Model.h"
#include <memory>

#include "scone/core/PropNode.h"
#include "scone/core/types.h"
#include "scone/optimization/Params.h"

namespace scone
{
	class SCONE_API Simulation
	{
	public:
		Simulation( const PropNode& props );
		virtual ~Simulation();

		Model& AddModel( const String& filename );
		size_t GetModelCount() { return m_Models.size(); }
		Model& GetModel( size_t idx = 0 );

		virtual void AdvanceSimulationTo( double time ) = 0;

		double max_simulation_time;
		double integration_accuracy;

	protected:
		std::vector< ModelUP > m_Models;

	private: // non-copyable and non-assignable
		Simulation( const Simulation& );
		Simulation& operator=( const Simulation& );
	};
}
