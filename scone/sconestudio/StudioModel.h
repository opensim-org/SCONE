#pragma once

#include "scone/sim/Model.h"
#include "simvis/scene.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/opt/Objective.h"

#include "flut/flag_set.hpp"
#include "simvis/arrow.h"

#include <thread>
#include <mutex>
#include <atomic>

#include "QSconeStorageDataModel.h"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( StudioModel );

	class StudioModel
	{
	public:
		enum ViewSettings { ShowForces, ShowMuscles, ShowGeometry, EnableShadows };
		typedef flut::flag_set< ViewSettings > ViewFlags;
 
		StudioModel( vis::scene &s, const String& par_file );
		virtual ~StudioModel();

		void UpdateVis( TimeInSeconds t );
		void EvaluateObjective();
		void EvaluateTo( TimeInSeconds t );

		const Storage<>& GetData() { return data; }
		sim::Model& GetSimModel() { return so->GetModel(); }
		cs::SimulationObjective& GetObjective() { return *so; }

		bool IsEvaluating() { return is_evaluating.load(); }
		std::mutex& GetDataMutex() { return data_mutex; }

		void SetViewFlags( const ViewFlags& f ) { view_flags = f; }
		void SetViewSetting( ViewSettings e, bool value );

	private:
		void InitVis( vis::scene& s );

		Storage<> data;
		cs::SimulationObjectiveUP so;
		String filename;

		ViewFlags view_flags;

		std::vector< size_t > state_data_index;

		vis::material bone_mat;
		vis::material arrow_mat;
		vis::material muscle_mat;

		std::thread eval_thread;
		std::mutex data_mutex;
		std::atomic< bool > is_evaluating;

		vis::group root;
		std::vector< std::vector< vis::mesh > > body_meshes;
		std::vector< vis::mesh > joints;
		std::vector< std::pair< vis::trail, vis::material > > muscles;
		std::vector< vis::arrow > forces;
		vis::mesh com;
		void InitStateDataIndices();
	};
}
