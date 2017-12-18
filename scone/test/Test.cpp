#define SCONE_ENABLE_PROFILING

#include "Test.h"

#include <fstream>

#include "scone/core/Profiler.h"
#include "scone/controllers/cs_tools.h"
#include "scone/optimization/Params.h"
#include "scone/core/Factories.h"
#include "scone/core/string_tools.h"
#include "scone/core/Factories.h"
#include "scone/core/math.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include "scone/optimization/opt_tools.h"
#include "scone/objectives/SimulationObjective.h"
#include "scone/model/model_tools.h"
#include "scone/model/Muscle.h"
#include "scone/model/Dof.h"
#include "scone/model/Side.h"
#include "scone/model/simbody/Model_Simbody.h"

using std::cout;
using std::endl;

#include "xo/time/timer.h"
#include "xo/system/log_sink.h"
#include "xo/system/log.h"
#include "xo/filesystem/filesystem.h"
using xo::timer;

namespace scone
{
	void OptimizationTest()
	{
		//PerformOptimization( "config/optimization_test.xml" );
	}

	void ModelTest()
	{
		SCONE_PROFILE_FUNCTION;
		const double simulation_time = 0.2;
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
			ParamInfo par;
			props.set( "Model.model_file", *iter );
			ModelUP m = CreateModel( props.get_child( "Model" ), par );

			log::DebugF( "Muscles=%d Bodies=%d Joints=%d Controllers=%d", m->GetMuscles().size(), m->GetBodies().size(), m->GetJoints().size(), m->GetControllers().size() );
			log::Debug( "Starting simulation..." );

			timer t;
			m->AdvanceSimulationTo( simulation_time );
			auto time = t.seconds();

			//std::cout << *m;
			//std::cout << "Total metabolic energy: " << m->GetTotalEnergyConsumption() << std::endl;
			//std::cout << "Metabolic energy rate per Kg: " << m->GetTotalEnergyConsumption() / simulation_time / m->GetMass() << std::endl;

			std::cout << "performance (x real-time): " << m->GetTime() / time << endl;

			m->WriteResult( path( *iter ).replace_extension( "simulation_test" ) );

			//if ( par.IsInConstructionMode() )
			//	par.SetMode( ParamSet::UpdateMode );
		}
	}

	void PlaybackTest( const path& filename )
	{
		xo::log::stream_sink cout_log( xo::log::trace_level );

		auto config_path = filename.parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			xo::current_path( config_path.parent_path() );

		const PropNode configProp = load_file_with_include( config_path ) ;
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
		ObjectiveUP obj = CreateObjective( objProp );
		SimulationObjective& so = dynamic_cast< SimulationObjective& >( *obj );
		auto model = so.CreateModelFromParFile( filename );

		SCONE_PROFILE_RESET;
		double result;
		timer t;
		result = so.EvaluateModel( *model );
		auto duration = t.seconds();

		// collect statistics
		PropNode stats;
		stats.set( "result", model->GetMeasure()->GetReport() );
		stats.set( "simulation time", model->GetTime() );
		stats.set( "performance (x real-time)", model->GetTime() / duration );

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

	void MuscleLengthTest()
	{
		PropNode props = load_file_with_include( "simulation_test.xml" );
		props[ "Model" ].set( "Model.model_file", String( "f2354.osim" ) );
		ParamInfo par; // empty parameter set
		ModelUP m = CreateModel( props.get_child( "Model" ), par );

		for ( int dof_val = -30; dof_val <= 30; dof_val += 5 )
		{
			for ( DofUP& dof: m->GetDofs() )
				dof->SetPos( dof_val, true );

			cout << "DOF offset = " << dof_val << endl;
			for ( MuscleUP& mus: m->GetMuscles() )
			{
				if ( GetSideFromName( mus->GetName() ) == RightSide )
				{
					Muscle& lmus = *FindByName( m->GetMuscles(), GetMirroredName( mus->GetName() ) );
					cout << stringf( "%l20s: %.3f\t%l20s: %.3f\tdelta=%.3f",
						mus->GetName().c_str(), mus->GetLength(), lmus.GetName(), lmus.GetLength(),
						std::abs( mus->GetLength() - lmus.GetLength() ) );
				}
			}
		}
	}

	void DofAxisTest()
	{
		PropNode props = load_file_with_include( "simulation_test.xml" );
		props[ "Model" ].set( "Model.model_file", String( "f2354.osim" ) );

		ParamInfo par; // empty parameter set
		ModelUP m = CreateModel( props.get_child( "Model" ), par );

		for ( DofUP& dof : m->GetDofs() )
			log::Info( dof->GetName() + ": " + to_str( dof->GetRotationAxis() ) );

		dynamic_cast<Model_Simbody&>( *m ).ValidateDofAxes();
	}
}
