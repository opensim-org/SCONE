#pragma once

#include "../sim/simbody/Simulation_Simbody.h"
#include "../sim/sim.h"
#include "../sim/Model.h"
#include "../core/Log.h"
#include "../core/Timer.h"
#include "../core/Propertyable.h"

using namespace scone;

void SimulationTest()
{
	const double simulation_time = 0.5;

	cs::RegisterFactoryTypes();
	PropNode props = ReadXmlFile( "config/simulation_test.xml" );

	std::vector< String > models;
	models.push_back( "models/ToyLandingModel.osim" );
	models.push_back( "models/ToyLandingModel_Millard2012Eq.osim" );
	//models.push_back( "models/ToyLandingModel_Millard2012Acc.osim" );
	opt::ParamSet par;

	// run all models
	for ( auto iter = models.begin(); iter != models.end(); ++iter )
	{
		props.Set( "Model.model_file", *iter );
		sim::ModelUP m = CreateFromPropNode< sim::Model >( props.GetChild( "Model" ) );
		m->ProcessParameters( par );

		Timer t;
		m->AdvanceSimulationTo( simulation_time );
		double time = t.GetTime();
		SCONE_LOG( "Simulation time: " << time << " (" << simulation_time / time << "x real-time)");

		if ( par.IsInConstructionMode() )
			par.SetMode( opt::ParamSet::UPDATE_MODE );
	}
	SCONE_LOG( "Done!" );
}
