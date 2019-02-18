/*
** Controller.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Controller.h"
#include "spot/par_tools.h"

namespace scone
{
	Controller::Controller( const PropNode& props, Params& par, Model& model, const Location& target_area ) :
	HasSignature( props )
	{
		INIT_PAR( props, par, start_time, 0.0 );
		INIT_PAR( props, par, stop_time, 0.0 ); // automatically stops after ~31000 years
		INIT_PROP( props, disabled_, false );
	}

	Controller::~Controller()
	{
	}

	bool Controller::UpdateControls( Model& model, double timestamp )
	{
		if ( IsActive( model, timestamp ) )
			return ComputeControls( model, timestamp );
		else return false;
	}

	bool Controller::UpdateAnalysis( const Model& model, double timestamp )
	{
		if ( IsActive( model, timestamp ) )
			return PerformAnalysis( model, timestamp );
		else return false;
	}
}
