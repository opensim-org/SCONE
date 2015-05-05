#pragma once

#include "../sim/simbody/Simulation_Simbody.h"
#include "../sim/sim.h"
#include "../sim/Model.h"
#include "../core/Log.h"
#include "../core/Timer.h"
#include "../core/InitFromPropNode.h"
#include "../sim/Factories.h"

#include <fstream>
#include "../core/Delayer.h"
#include "../core/Storage.h"

using namespace scone;

void DelayTest()
{
	std::ofstream str( "delay_test.txt" );

	double delay = 1.0;
	DelayedReal dv( delay );
	Storage store;
	for ( double t = 0.0; t < 20.0; t += ( 1 + rand() % 200 ) / 100.0 )
	{
		Real v = cos( t );

		if ( t < 15 )
		{
			dv.AddSample( t, v );
			store.SetValue( t, "Cos", v );
			store.SetValue( t, "Sin", sin( t ) );
			store.SetValue( t, "SQRT", sqrt( t ) );
		}

		str << t << "\t" << v << "\t" << dv.GetDelayedValue( t );

		for ( size_t idx = 0; idx < store.GetChannelCount(); ++idx )
		{
			str << "\t" << store.GetInterpolatedValue( t, idx );
			str << "\t" << store.GetInterpolatedValue( t - delay, idx );
		}
		str << std::endl;
	}
}

void SimulationTest()
{
	const double simulation_time = 0.2;

	cs::RegisterFactoryTypes();
	PropNode props = ReadPropNodeFromXml( "simulation_test.xml" );

	std::vector< String > models;
	//models.push_back( "../models/f1024.osim" );
	models.push_back( "../models/f2354.osim" );
	//models.push_back( "models/test/gait2354.osim" );
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
		sim::ModelUP m = sim::CreateModel( props.GetChild( "Model" ), par );

		log::DebugF( "Muscles=%d Bodies=%d Joints=%d Controllers=%d", m->GetMuscleCount(), m->GetBodyCount(), m->GetJoints().size(), m->GetControllers().size() );
		log::Debug( "Starting simulation..." );

		Timer t;
		m->AdvanceSimulationTo( simulation_time );
		double time = t.GetTime();

		//std::cout << *m;

		std::cout << "Total metabolic energy: " << m->GetTotalEnergyConsumption() << std::endl;
		std::cout << "Metabolic energy rate per Kg: " << m->GetTotalEnergyConsumption() / simulation_time / m->GetMass() << std::endl;

		//if ( par.IsInConstructionMode() )
		//	par.SetMode( opt::ParamSet::UpdateMode );
	}
}
