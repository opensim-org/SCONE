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

		void Update( TimeInSeconds t );
		void EvaluateObjective();
		const Storage< Real, TimeInSeconds >& GetData() { return data; }

		cs::SimulationObjective& GetSimulationObjective() { return dynamic_cast< cs::SimulationObjective& >( *objective ); }

	private:
		void CreateObjective( const String& par_file );
		void InitVis( vis::scene& s );

		sim::ModelUP model;
		Storage< Real, TimeInSeconds > data;
		String filename;
		opt::ObjectiveUP objective;
		PropNode statistics;

		std::vector< vis::mesh > bodies;
		std::vector< vis::mesh > joints;
		std::vector< vis::path > muscles;
		std::vector< vis::arrow > forces;
		vis::mesh com;
	};
}
