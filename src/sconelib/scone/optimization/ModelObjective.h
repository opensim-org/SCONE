/*
** ModelObjective.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/optimization/Objective.h"
#include "scone/model/Model.h"
#include "scone/core/Factories.h"

namespace scone
{
	/// Base class for Objectives that involve Models.
	class SCONE_API ModelObjective : public Objective
	{
	public:
		ModelObjective( const PropNode& props, const path& find_file_folder );
		virtual ~ModelObjective() = default;

		virtual result<fitness_t> evaluate( const SearchPoint& point, const xo::stop_token& st ) const override;
		virtual result<fitness_t> EvaluateModel( Model& m, const xo::stop_token& st ) const;

		virtual void AdvanceSimulationTo( Model& m, TimeInSeconds t ) const = 0;
		virtual TimeInSeconds GetDuration() const = 0;
		virtual fitness_t GetResult( Model& m ) const = 0;
		virtual PropNode GetReport( Model& m ) const = 0;

		virtual ModelUP CreateModelFromParams( Params& point ) const;
		ModelUP CreateModelFromParFile( const path& parfile ) const;

		virtual std::vector<path> WriteResults( const path& file_base ) override;

		const Model& GetModel() const { return *model_; }
		Model& GetModel() { return *model_; }

	protected:
		FactoryProps model_props;
		FactoryProps controller_props;
		FactoryProps measure_props;

		ModelUP model_;
		String signature_; // cached variable, because we need to create a model to get the signature
		virtual String GetClassSignature() const override { return signature_; }
		TimeInSeconds evaluation_step_size_;
	};

	/// Create ModelObjective from a PropNode
	/// CAUTION: scenario_pn is required afterwards by CreateModelFromParams()
	ModelObjectiveUP SCONE_API CreateModelObjective( const PropNode& scenario_pn, const path& dir );
}
