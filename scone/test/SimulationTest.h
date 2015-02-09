#pragma once

#include "../sim/simbody/Simulation_Simbody.h"
#include "../sim/sim.h"
#include "../sim/Model.h"
#include "../core/Log.h"
#include "../core/Timer.h"
#include "../core/InitFromPropNode.h"

using namespace scone;

void SimulationTest()
{
	const double simulation_time = 1.0;

	cs::RegisterFactoryTypes();
	PropNode props = ReadXmlFile( "config/simulation_test.xml" );

	std::vector< String > models;
	models.push_back( "models/jump1024.osim" );
	//models.push_back( "models/gait2354.osim" );
	//models.push_back( "models/jump2354.osim" );
	//models.push_back( "models/gait1018.osim" );
	//models.push_back( "models/gait1024.osim" );
	//models.push_back( "models/ToyLandingModel.osim" );
	//models.push_back( "models/ToyLandingModel_Millard2012Eq.osim" );
	//models.push_back( "models/ToyLandingModel_Millard2012Acc.osim" );

	// run all models
	for ( auto iter = models.begin(); iter != models.end(); ++iter )
	{
		opt::ParamSet par;
		props.Set( "Model.model_file", *iter );
		sim::ModelUP m = CreateFromPropNode< sim::Model >( props.GetChild( "Model" ) );
		m->ProcessParameters( par );

		SCONE_LOG( "Muscles=" << m->GetMuscleCount() << " Bodies=" << m->GetBodyCount() << " Joints=" << m->GetJoints().size() );
		SCONE_LOG( "Starting simulation..." );

		Timer t;
		m->AdvanceSimulationTo( simulation_time );
		double time = t.GetTime();
		SCONE_LOG( "Simulation time: " << time << " (" << simulation_time / time << "x real-time)");

		std::cout << *m;

		//if ( par.IsInConstructionMode() )
		//	par.SetMode( opt::ParamSet::UpdateMode );
	}
	SCONE_LOG( "Done!" );
}
