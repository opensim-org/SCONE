#define SCONE_ENABLE_PROFILING
#include "scone/core/Profiler.h"

#include "Test.h"
#include "scone/cs/cs_tools.h"
#include "scone/opt/ParamSet.h"
#include "scone/sim/Factories.h"
#include "scone/core/string_tools.h"
#include <fstream>

#include <boost/filesystem.hpp>
#include "scone/opt/Factories.h"
#include "scone/opt/opt_tools.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"
#include "scone/sim/sim_tools.h"
#include "scone/sim/Muscle.h"
#include "boost/format.hpp"
#include "scone/sim/Dof.h"
#include "scone/sim/Side.h"
#include "scone/sim/simbody/Model_Simbody.h"

namespace bfs = boost::filesystem;
using std::cout;
using std::endl;

#include <flut/timer.hpp>
#include "flut/system/log_sink.hpp"
#include "flut/system/log.hpp"
using flut::timer;

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
		PropNode props = load_file_with_include( "simulation_test.xml" );

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
			props.set( "Model.model_file", *iter );
			sim::ModelUP m = sim::CreateModel( props.get_child( "Model" ), par );

			log::DebugF( "Muscles=%d Bodies=%d Joints=%d Controllers=%d", m->GetMuscles().size(), m->GetBodies().size(), m->GetJoints().size(), m->GetControllers().size() );
			log::Debug( "Starting simulation..." );

			timer t;
			m->AdvanceSimulationTo( simulation_time );
			auto time = t.seconds();

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
		flut::log::stream_sink cout_log( flut::log::trace_level );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		opt::ParamSet par( filename );

		bfs::path config_path = bfs::path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );

		const PropNode configProp = load_file_with_include( config_path.string() ) ;
		PropNode objProp = configProp[ "Optimizer" ][ "Objective" ];

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

		SCONE_PROFILE_RESET;
		double result;
		timer t;
		result = obj->Evaluate();
		auto duration = t.seconds();

		// collect statistics
		PropNode stats;
		stats.set( "result", so.GetMeasure().GetReport() );
		stats.set( "simulation time", so.GetModel().GetTime() );
		stats.set( "performance (x real-time)", so.GetModel().GetTime() / duration );

		cout << "--- Evaluation report ---" << endl;
		cout << stats << endl;

		// write results
		//obj->WriteResults( bfs::path( filename ).replace_extension().string() );
	}

	void DelayTest()
	{
		std::ofstream str( "delay_test.txt" );

		double delay = 5.0;
		//DelayedReal dv( delay );
		Storage< double > store;
		for ( double t = 0.0; t < 20.0; t += ( 1 + rand() % 100 ) / 100.0 )
		{
			Real v = cos( t );

			if ( t < 15 )
			{
				store.AddFrame( t );
				store.Back()[ "Cos" ] = v;

				for ( int i = 1; i < 6; ++i )
					store.Back()[ "Test" + to_str( i ) ] = store.GetInterpolatedValue( t - 1.0, i - 1 );
			}
			str << t << "\t" << v << "\t" << store.GetInterpolatedValue( t - delay, 0 );

			for ( size_t idx = 1; idx < store.GetChannelCount(); ++idx )
			{
				str << "\t" << store.GetInterpolatedValue( t, idx );
				//str << "\t" << store.GetInterpolatedValue( t - delay, idx );
			}
			str << std::endl;
		}
	}

	void PerformanceTest( const String& filename )
	{
		flut::log::stream_sink cout_log( flut::log::trace_level );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		opt::ParamSet par( filename );
		bfs::path config_path = bfs::path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );

		const PropNode configProp = load_file_with_include( config_path.string() ) ;
		PropNode objProp = configProp[ "Optimizer" ][ "Objective" ];

		// override some variables
		objProp.set( "max_duration", 1 );
		objProp.set( "Model.integration_accuracy", 1e-3 );
		//objProp.Set( "Model.use_fixed_control_step_size", true );
		//objProp.Set( "Model.fixed_control_step_size", 0.01 );
		//objProp.Set( "Model.max_step_size", 0.001 );
		//objProp.Set( "Model.integration_method", String("SemiExplicitEuler2") );
		objProp.set("Model.integration_method", String("RungeKuttaMerson"));

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );
		SCONE_PROFILE_RESET;
		double result;
		timer t;
		result = obj->Evaluate();
		auto duration = t.seconds();

		// collect statistics
		PropNode stats;
		stats.set( "result", so.GetMeasure().GetReport() ).set_value( result );
		stats.set( "simulation time", so.GetModel().GetTime() );
		stats.set( "performance (x real-time)", so.GetModel().GetTime() / duration );
		cout << "--- Evaluation report ---" << endl;
		cout << stats << endl;

#ifdef SCONE_ENABLE_PROFILING
		cout << "Profile report:" << endl;
		cout << Profiler::GetGlobalInstance().GetReport();
#endif
		cout << "All done!" << endl;
	}

	void SimulationObjectiveTest( const String& filename )
	{
		flut::log::stream_sink cout_log( flut::log::trace_level );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();

		opt::ParamSet par; // empty parameter set
		const PropNode configProp = load_file_with_include( filename ) ;
		const PropNode& objProp = configProp[ "Optimizer" ][ "Objective" ];

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

		// reset profiler
		SCONE_PROFILE_RESET;

		timer t;
		double result = obj->Evaluate();
		auto duration = t.seconds();

		// collect statistics
		PropNode stats;
		stats.set( "result", so.GetMeasure().GetReport() ).set_value( result );
		stats.set( "simulation time", so.GetModel().GetTime() );
		stats.set( "performance (x real-time)", so.GetModel().GetTime() / duration );
		cout << "--- Evaluation report ---" << endl;
		cout << stats << endl;

#ifdef SCONE_ENABLE_PROFILING
		cout << "Profile report:" << endl;
		cout << SCONE_PROFILE_REPORT;
#endif
		cout << "All done!" << endl;
	}

	void MuscleLengthTest()
	{
		cs::RegisterFactoryTypes();
		PropNode props = load_file_with_include( "simulation_test.xml" );
		props[ "Model" ].set( "Model.model_file", String( "f2354.osim" ) );
		opt::ParamSet par; // empty parameter set
		sim::ModelUP m = sim::CreateModel( props.get_child( "Model" ), par );

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
						% std::abs( mus->GetLength() - lmus.GetLength() ) << endl;
				}
			}
		}
	}

	void DofAxisTest()
	{
		cs::RegisterFactoryTypes();
		PropNode props = load_file_with_include( "simulation_test.xml" );
		props[ "Model" ].set( "Model.model_file", String( "f2354.osim" ) );

		opt::ParamSet par; // empty parameter set
		sim::ModelUP m = sim::CreateModel( props.get_child( "Model" ), par );

		for ( sim::DofUP& dof : m->GetDofs() )
			log::Info( dof->GetName() + ": " + to_str( dof->GetRotationAxis() ) );

		dynamic_cast<sim::Model_Simbody&>( *m ).ValidateDofAxes();
	}
}
