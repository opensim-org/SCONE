#pragma once

#include "scone/core/system_tools.h"
#include "scone/model/Model.h"
#include "simvis/scene.h"
#include "scone/objectives/SimulationObjective.h"
#include "scone/optimization/Objective.h"

#include "flut/flag_set.hpp"
#include "simvis/arrow.h"

#include "SconeStorageDataModel.h"
#include "simvis/axes.h"
#include "scone/model/State.h"
#include "scone/core/types.h"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( StudioModel );

	class StudioModel
	{
	public:
		enum ViewSettings { ShowForces, ShowMuscles, ShowGeometry, EnableShadows, ShowAxes };
		typedef flut::flag_set< ViewSettings > ViewFlags;
 
		StudioModel( vis::scene &s, const path& filename, bool force_evaluation = false );
		virtual ~StudioModel();

		void UpdateVis( TimeInSeconds t );
		void UpdateForceVis( Index force_idx, Vec3 cop, Vec3 force );

		void EvaluateObjective();
		void EvaluateTo( TimeInSeconds t );
		void FinalizeEvaluation( bool output_results );

		const Storage<>& GetData() { return data; }
		Model& GetSimModel() { return so->GetModel(); }
		SimulationObjective& GetObjective() { return *so; }

		bool IsEvaluating() { return is_evaluating; }
		TimeInSeconds GetTime() const { return so->GetModel().GetTime(); }

		void SetViewSetting( ViewSettings e, bool value );
		void ApplyViewSettings( const ViewFlags& f );

	private:
		void InitVis( vis::scene& s );

		Storage<> data;
		SimulationObjectiveUP so;
		path filename;

		ViewFlags view_flags;

		std::vector< size_t > state_data_index;
		scone::State model_state;

		vis::material bone_mat;
		vis::material arrow_mat;
		vis::material muscle_mat;

		bool is_evaluating;

		vis::group root;
		std::vector< std::vector< vis::mesh > > body_meshes;
		std::vector< vis::mesh > joints;
		std::vector< std::pair< vis::trail, vis::material > > muscles;
		std::vector< vis::arrow > forces;
		std::vector< vis::axes > body_axes;
		vis::mesh com;
		void InitStateDataIndices();
	};
}
