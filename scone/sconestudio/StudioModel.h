#pragma once

#include "scone/sim/Model.h"
#include "simvis/scene.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/opt/Objective.h"

#include "simvis/arrow.h"
#include <thread>
#include <mutex>
#include <atomic>

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( StudioModel );

	class StudioModel
	{
	public:
		StudioModel( vis::scene &s, const String& par_file );
		virtual ~StudioModel();

		void UpdateVis( TimeInSeconds t );
		void EvaluateObjective();

		const Storage<>& GetData() { return data; }
		sim::Model& GetSimModel() { return so->GetModel(); }

		bool IsEvaluating() { return is_evaluating.load(); }
		std::mutex& GetDataMutex() { return data_mutex; }

	private:
		void InitVis( vis::scene& s );
		void SetModelStateFromDataFrame( const Storage< Real, TimeInSeconds >::Frame& f );

		Storage<> data;
		cs::SimulationObjectiveUP so;
		String filename;

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
		std::vector< std::pair< vis::path, vis::material > > muscles;
		std::vector< vis::arrow > forces;
		vis::mesh com;
		void InitStateDataIndices();
	};
}
