/*
** ModelObjective.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/optimization/Objective.h"
#include "scone/model/Model.h"

namespace scone
{
	/// Base class for Objectives that involve Models.
	class SCONE_API ModelObjective : public Objective
	{
	public:
		ModelObjective( const PropNode& props );
		virtual ~ModelObjective() {}

		/// Model to be used for the simulation.
		PropNode model;

		virtual fitness_t evaluate( const SearchPoint& point ) const override;

		virtual fitness_t EvaluateModel( Model& m ) const;
		virtual void AdvanceSimulationTo( Model& m, TimeInSeconds t ) const = 0;

		virtual fitness_t GetResult( Model& m ) const = 0;
		virtual PropNode GetReport( Model& m ) const = 0;

		virtual TimeInSeconds GetDuration() const = 0;
		virtual ModelUP CreateModelFromParams( Params& point ) const;
		ModelUP CreateModelFromParFile( const path& parfile ) const;

		virtual std::vector<path> WriteResults( const path& file_base ) override;
	};

	ModelObjectiveUP SCONE_API CreateModelObjective( const path& config_file );
}
