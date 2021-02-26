/*
** Simulation.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Simulation.h"

namespace scone
{
	Simulation::Simulation( const PropNode& props ) :
		max_simulation_time( 0.0 ),
		integration_accuracy( 0.0 )
	{
		INIT_PROP( props, integration_accuracy, 0.00001 );
		INIT_PROP( props, max_simulation_time, 10000.0 );
	}

	Simulation::~Simulation()
	{

	}

	Model& Simulation::GetModel( size_t idx )
	{
		SCONE_ASSERT( idx < m_Models.size() );
		return *m_Models[ idx ];
	}
}
