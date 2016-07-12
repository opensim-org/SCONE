#pragma once

#include "scone/sim/Model.h"
#include "simvis/scene.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/opt/Objective.h"

namespace scone
{
	SCONE_DECLARE_CLASS_AND_PTR( StudioModel );

	class StudioModel
	{
	public:
		StudioModel( vis::scene &s, const String& par_file );
		virtual ~StudioModel() {}

		void UpdateVis( TimeInSeconds t );
		void EvaluateObjective();

		const Storage< Real, TimeInSeconds >& GetData() { return data; }
		sim::Model& GetSimModel() { return so->GetModel(); }

	private:
		void InitModel( const String& par_file );
		void InitVis( vis::scene& s );
		void SetModelStateFromDataFrame( const Storage< Real, TimeInSeconds >::Frame& f );

		Storage< Real, TimeInSeconds > data;
		cs::SimulationObjectiveUP so;
		String filename;

		std::vector< size_t > state_data_index;

		std::vector< std::vector< vis::mesh > > body_meshes;
		std::vector< vis::mesh > joints;
		std::vector< vis::path > muscles;
		std::vector< vis::arrow > forces;
		vis::mesh com;
	};
}
