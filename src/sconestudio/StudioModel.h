/*
** StudioModel.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/system_tools.h"
#include "scone/model/Model.h"
#include "simvis/scene.h"
#include "scone/optimization/SimulationObjective.h"
#include "scone/optimization/Objective.h"

#include "xo/container/flag_set.h"
#include "simvis/arrow.h"

#include "SconeStorageDataModel.h"
#include "simvis/axes.h"
#include "scone/model/State.h"
#include "scone/core/types.h"
#include "scone/optimization/ModelObjective.h"
#include "simvis/color_gradient.h"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( StudioModel );

	class StudioModel
	{
	public:
		enum ViewSettings { ShowForces, ShowMuscles, ShowGeometry, ShowAxes, ShowContactGeom, EnableShadows };
		typedef xo::flag_set< ViewSettings > ViewFlags;
 
		StudioModel( vis::scene &s, const path& filename, bool force_evaluation = false );
		virtual ~StudioModel();

		void UpdateVis( TimeInSeconds t );

		void EvaluateTo( TimeInSeconds t );
		void FinalizeEvaluation( bool output_results );

		const Storage<>& GetData() { return data; }
		Model& GetSimModel() { return *model; }
		ModelObjective& GetObjective() { return *model_objective; }

		bool IsEvaluating() const { return is_evaluating; }
		TimeInSeconds GetTime() const { return model->GetTime(); }
		TimeInSeconds GetMaxTime() const { return IsEvaluating() ? model_objective->GetDuration() : data.Back().GetTime(); }

		void ApplyViewSettings( const ViewFlags& f );
		const path& GetFileName() { return filename; }

	private:
		struct MuscleVis
		{
			vis::trail ten1;
			vis::trail ten2;
			vis::trail ce;
			vis::material mat;
		};

		void InitVis( vis::scene& s );
		void UpdateForceVis( index_t force_idx, Vec3 cop, Vec3 force );
		void UpdateMuscleVis( const class Muscle& mus, MuscleVis& vis );

		Storage<> data;
		ModelObjectiveUP model_objective;
		ModelUP model;
		path filename;

		ViewFlags view_flags;

		std::vector< size_t > state_data_index;
		scone::State model_state;

		vis::material bone_mat;
		vis::material arrow_mat;
		vis::material muscle_mat;
		vis::material tendon_mat;


		vis::color_gradient muscle_gradient;
		bool is_evaluating;

		vis::group root;
		std::vector< vis::mesh > body_meshes;
		std::vector< vis::mesh > joints;
		std::vector< MuscleVis > muscles;
		std::vector< vis::arrow > forces;
		std::vector< vis::axes > body_axes;
		std::vector< vis::group > bodies;
		std::vector< vis::mesh > contact_geoms;
		vis::mesh com;
		void InitStateDataIndices();
	};
}
