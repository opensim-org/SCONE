/*
** types.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/memory_tools.h"
#include "xo/xo_types.h"
#include <string>

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( Function );

	SCONE_DECLARE_CLASS_AND_PTR( SimulationObjective );
	SCONE_DECLARE_CLASS_AND_PTR( ImitationObjective );
	SCONE_DECLARE_CLASS_AND_PTR( ModelObjective );
	SCONE_DECLARE_CLASS_AND_PTR( FeedForwardController );
	SCONE_DECLARE_CLASS_AND_PTR( ReflexController );
	SCONE_DECLARE_CLASS_AND_PTR( MuscleReflex );
	SCONE_DECLARE_CLASS_AND_PTR( Reflex );
	SCONE_DECLARE_CLASS_AND_PTR( Measure );
	SCONE_DECLARE_CLASS_AND_PTR( HeightMeasure );
	SCONE_DECLARE_CLASS_AND_PTR( MetaReflexController );
	SCONE_DECLARE_CLASS_AND_PTR( MetaReflexDof );
	SCONE_DECLARE_CLASS_AND_PTR( MetaReflexMuscle );
	SCONE_DECLARE_CLASS_AND_PTR( MetaReflexVirtualMuscle );

	// forward declarations
	SCONE_DECLARE_CLASS_AND_PTR( Simulation );
	SCONE_DECLARE_CLASS_AND_PTR( Model );
	SCONE_DECLARE_CLASS_AND_PTR( Body );
	SCONE_DECLARE_CLASS_AND_PTR( ContactForce );
	SCONE_DECLARE_CLASS_AND_PTR( ContactGeometry );
	SCONE_DECLARE_CLASS_AND_PTR( Joint );
	SCONE_DECLARE_CLASS_AND_PTR( Dof );
	SCONE_DECLARE_CLASS_AND_PTR( Link );
	SCONE_DECLARE_CLASS_AND_PTR( Location );
	SCONE_DECLARE_CLASS_AND_PTR( Actuator );
	SCONE_DECLARE_CLASS_AND_PTR( Muscle );
	SCONE_DECLARE_CLASS_AND_PTR( Controller );
	SCONE_DECLARE_CLASS_AND_PTR( Leg );
	SCONE_DECLARE_STRUCT_AND_PTR( Sensor );
	SCONE_DECLARE_STRUCT_AND_PTR( SensorDelayAdapter );
	SCONE_DECLARE_CLASS_AND_PTR( Optimizer );
	SCONE_DECLARE_CLASS_AND_PTR( Objective );

	// types
#ifdef SCONE_SINGLE_PRECISION_FLOAT
	typedef float Real;
#else
	typedef double Real;
#endif

	using String = std::string;

	// index type
	using xo::index_t;
	using xo::no_index;
	using std::string;
	const index_t NoIndex = size_t( -1 );
	using namespace xo::literals;

	typedef double TimeInSeconds;
	typedef double Frequency;
}
