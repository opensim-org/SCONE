#define SCONE_ENABLE_PROFILING

#include "Test.h"
#include "scone/cs/cs.h"
#include "scone/opt/ParamSet.h"
#include "scone/sim/Factories.h"
#include "scone/core/Timer.h"
#include <fstream>

#include <boost/filesystem.hpp>
#include "scone/opt/Factories.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/core/Profiler.h"
#include "scone/core/system.h"
#include "scone/sim/sim.h"
#include "scone/sim/Muscle.h"
#include "boost/format.hpp"
#include "scone/sim/Dof.h"
#include "scone/cs/tools.h"
#include "scone/sim/Side.h"
#include "scone/sim/simbody/Model_Simbody.h"

namespace bfs = boost::filesystem;
using std::cout;
using std::endl;

namespace scone
{
	void OptimizationTest()
	{
		// register new objective
		//opt::GetObjectiveFactory();
		//cs::PerformOptimization( "config/optimization_test.xml" );
	}

	void ModelTest()
	{
		SCONE_PROFILE_SCOPE;
		const double simulation_time = 0.2;

		cs::RegisterFactoryTypes();
		PropNode props = ReadPropNodeFromXml( "simulation_test.xml" );

		std::vector< String > models;
		models.push_back( "../models/f1024.osim" );
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

			log::DebugF( "Muscles=%d Bodies=%d Joints=%d Controllers=%d", m->GetMuscles().size(), m->GetBodies().size(), m->GetJoints().size(), m->GetControllers().size() );
			log::Debug( "Starting simulation..." );

			Timer t;
			m->AdvanceSimulationTo( simulation_time );
			auto time = t.GetTime();

			//std::cout << *m;
			//std::cout << "Total metabolic energy: " << m->GetTotalEnergyConsumption() << std::endl;
			//std::cout << "Metabolic energy rate per Kg: " << m->GetTotalEnergyConsumption() / simulation_time / m->GetMass() << std::endl;

			std::cout << "performance (x real-time): " << m->GetTime() / time << endl;

			m->WriteData( get_filename_without_ext( *iter ) + "_simulation_test" );

			//if ( par.IsInConstructionMode() )
			//	par.SetMode( opt::ParamSet::UpdateMode );
		}
	}

	void PlaybackTest( const String& filename )
	{
		log::SetLevel( log::TraceLevel );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		opt::ParamSet par( filename );

		bfs::path config_path = bfs::path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );

		PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
		PropNode objProp = configProp.GetChild( "Optimizer.Objective" );

		// override some variables
		//objProp.Set( "max_duration", 1 );
		//objProp.Set( "Model.integration_accuracy", 1e-3 );
		//objProp.Set( "Model.use_fixed_control_step_size", true );
		//objProp.Set( "Model.fixed_control_step_size", 0.01 );
		//objProp.Set( "Model.max_step_size", 0.001 );
		//objProp.Set( "Model.integration_method", String("SemiExplicitEuler2") );
		//objProp.Set("Model.integration_method", String("RungeKuttaMerson"));

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

		Profiler::GetGlobalInstance().Reset();

		double result;
		Timer timer;
		result = obj->Evaluate();

		timer.Pause();

		// collect statistics
		PropNode stats;
		stats.Clear();
		stats.Set( "result", result );
		stats.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
		stats.Set( "simulation time", so.GetModel().GetTime() );
		stats.Set( "performance (x real-time)", so.GetModel().GetTime() / timer.GetTime() );

		cout << "--- Evaluation report ---" << endl;
		cout << stats << endl;

		// write results
		//obj->WriteResults( bfs::path( filename ).replace_extension().string() );
	}

	void DelayTest()
	{
		std::ofstream str( "delay_test.txt" );

		double delay = 5.0;
		DelayedReal dv( delay );
		Storage< double > store;
		for ( double t = 0.0; t < 20.0; t += ( 1 + rand() % 100 ) / 100.0 )
		{
			Real v = cos( t );

			if ( t < 15 )
			{
				dv.AddSample( t, v );
				store.AddFrame( t );
				store.Back()[ "Cos" ] = v;

				for ( int i = 1; i < 6; ++i )
					store.Back()[ "Test" + make_str( i ) ] = store.GetInterpolatedValue( t - 1.0, i - 1 );
			}
			str << t << "\t" << v << "\t" << dv.GetDelayedValue( t ) << "\t" << store.GetInterpolatedValue( t - delay, 0 );

			for ( size_t idx = 1; idx < store.GetChannelCount(); ++idx )
			{
				str << "\t" << store.GetInterpolatedValue( t, idx );
				//str << "\t" << store.GetInterpolatedValue( t - delay, idx );
			}
			str << std::endl;
		}
	}

	void XmlParseTest()
	{
		PropNode prop;
		prop.FromXmlFile( "config/optimization_test.xml" );
		prop.ToInfoFile( "config/optimization_test.info" );
		std::cout << prop;
	}

	void PerformanceTest( const String& filename )
	{
		log::SetLevel( log::InfoLevel );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		opt::ParamSet par( filename );
		bfs::path config_path = bfs::path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );

		PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
		PropNode objProp = configProp.GetChild( "Optimizer.Objective" );

		// override some variables
		objProp.Set( "max_duration", 1 );
		objProp.Set( "Model.integration_accuracy", 1e-3 );
		//objProp.Set( "Model.use_fixed_control_step_size", true );
		//objProp.Set( "Model.fixed_control_step_size", 0.01 );
		//objProp.Set( "Model.max_step_size", 0.001 );
		//objProp.Set( "Model.integration_method", String("SemiExplicitEuler2") );
		objProp.Set("Model.integration_method", String("RungeKuttaMerson"));

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

		Profiler::GetGlobalInstance().Reset();
		Timer timer;
		double result;

		timer.Restart();
		result = obj->Evaluate();
		timer.Pause();

		// collect statistics
		PropNode stats;
		stats.Clear();
		stats.Set( "result", result );
		stats.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
		stats.Set( "simulation time", so.GetModel().GetTime() );
		stats.Set( "performance (x real-time)", so.GetModel().GetTime() / timer.GetTime() );
		cout << "--- Evaluation report ---" << endl;
		cout << stats << endl;

		cout << "Profile report:" << endl;
		cout << Profiler::GetGlobalInstance().GetReport();
		cout << "All done!" << endl;
	}

	void SimulationObjectiveTest( const String& filename )
	{
		log::SetLevel( log::TraceLevel );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		opt::ParamSet par; // empty parameter set
		PropNode configProp = ReadPropNodeFromXml( filename ) ;
		PropNode objProp = configProp.GetChild( "Optimizer.Objective" );

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

		// reset profiler
		Profiler::GetGlobalInstance().Reset();

		Timer timer;
		double result = obj->Evaluate();
		timer.Pause();

		// collect statistics
		PropNode stats;
		stats.Clear();
		stats.Set( "result", result );
		stats.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
		stats.Set( "simulation time", so.GetModel().GetTime() );
		stats.Set( "performance (x real-time)", so.GetModel().GetTime() / timer.GetTime() );
		cout << "--- Evaluation report ---" << endl;
		cout << stats << endl;

		cout << "Profile report:" << endl;
		cout << Profiler::GetGlobalInstance().GetReport();
		cout << "All done!" << endl;
	}

	void MuscleLengthTest()
	{
		cs::RegisterFactoryTypes();
		PropNode props = ReadPropNodeFromXml( "simulation_test.xml" );
		props.Set( "Model.model_file", "gait2354.osim" );
		opt::ParamSet par; // empty parameter set
		sim::ModelUP m = sim::CreateModel( props.GetChild( "Model" ), par );

		for ( int dof_val = -30; dof_val <= 30; dof_val += 5 )
		{
			for ( sim::DofUP& dof: m->GetDofs() )
				dof->SetPos( dof_val, true );

			cout << "DOF offset = " << dof_val << endl;
			for ( sim::MuscleUP& mus: m->GetMuscles() )
			{
				if ( GetSide( mus->GetName() ) == RightSide )
				{
					sim::Muscle& lmus = *FindByName( m->GetMuscles(), GetMirroredName( mus->GetName() ) );
					cout << boost::format( "%l20s: %.3f\t%l20s: %.3f\tdelta=%.3f" )
						% mus->GetName() % mus->GetLength() % lmus.GetName() % lmus.GetLength()
						% abs( mus->GetLength() - lmus.GetLength() ) << endl;
				}
			}
		}
	}

	void DofAxisTest()
	{
		cs::RegisterFactoryTypes();
		PropNode props = ReadPropNodeFromXml( "simulation_test.xml" );
		props.Set( "Model.model_file", "gait2354.osim" );
		opt::ParamSet par; // empty parameter set
		sim::ModelUP m = sim::CreateModel( props.GetChild( "Model" ), par );

		for ( sim::DofUP& dof : m->GetDofs() )
			log::Info( dof->GetName() + ": " + make_str( dof->GetRotationAxis() ) );

		dynamic_cast<sim::Model_Simbody&>( *m ).ValidateDofAxes();
	}
}
