/*
** StudioModel.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/system_tools.h"
#include "scone/model/Model.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/optimization/Objective.h"
#include "scone/model/State.h"
#include "scone/core/types.h"
#include "scone/optimization/ModelObjective.h"
#include "scone/optimization/Optimizer.h"

#include "ModelVis.h"
#include "qt_convert.h"

namespace scone
{
	class StudioModel
	{
	public:
		StudioModel( vis::scene& s, const path& filename );
		virtual ~StudioModel();

		void UpdateVis( TimeInSeconds t );
		void EvaluateTo( TimeInSeconds t );

		void AbortEvaluation();

		const Storage<>& GetData() { return storage_; }
		bool HasModel() const { return bool( model_ ); }
		bool HasData() const { return !storage_.IsEmpty() && !state_data_index.empty(); }

		Model& GetModel() { return *model_; }
		ModelObjective& GetModelObjective() const { return *model_objective_; }

		bool IsEvaluating() const { return status_ == Status::Evaluating; }
		bool IsReady() const { return status_ == Status::Ready; }
		bool IsValid() const { return status_ != Status::Error; }

		TimeInSeconds GetTime() const { return model_ ? model_->GetTime() : 0.0; }
		TimeInSeconds GetMaxTime() const;

		void ApplyViewSettings( const ModelVis::ViewSettings& f );
		Vec3 GetFollowPoint() const;

		const path& GetFileName() const { return filename_; }
		QString GetScenarioFileName() const { return to_qt( scenario_filename_ ); }
		const PropNode& GetScenarioProps() const { return scenario_pn_; }

		enum class Status { Initializing, Evaluating, Ready, Aborted, Error };
		Status GetStatus() const { return status_; }

	private:
		void FinalizeEvaluation();
		void InvokeError( const String& message );

		// visualizer
		u_ptr<ModelVis> vis_;

		// model / scenario data
		Storage<> storage_;
		OptimizerUP optimizer_;
		Objective* objective_;
		ModelObjective* model_objective_;
		ModelUP model_;
		path filename_;
		path scenario_filename_;
		PropNode scenario_pn_;

		Status status_;

		//bool is_evaluating_;

		// model state
		std::vector< size_t > state_data_index;
		scone::State model_state;
		void InitStateDataIndices();
	};
}
