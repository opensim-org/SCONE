#include "cs_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/Profiler.h"
#include "scone/opt/Optimizer.h"
#include "SimulationObjective.h"
#include "FeedForwardController.h"
#include "HeightMeasure.h"
#include "GaitMeasure.h"
#include "EffortMeasure.h"
#include "scone/sim/Factories.h"
#include "scone/sim/sim_tools.h"
#include "scone/opt/Factories.h"
#include "Factories.h"
#include "PieceWiseConstantFunction.h"
#include "PieceWiseLinearFunction.h"
#include "Polynomial.h"
#include "GaitStateController.h"
#include "ReflexController.h"
#include "DofLimitMeasure.h"
#include "CompositeMeasure.h"
#include "GaitCycleMeasure.h"
#include "DofReflex.h"
#include "MuscleReflex.h"
#include "ConditionalMuscleReflex.h"
#include "JumpMeasure.h"
#include "TimeStateController.h"
#include "MetaReflexController.h"
#include "JointLoadMeasure.h"
#include "scone/opt/opt_tools.h"
#include "scone/sim/simbody/sim_simbody.h"
#include "PointMeasure.h"

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <flut/timer.hpp>
#include "flut/prop_node_tools.hpp"
using flut::timer;

namespace scone
{
	namespace cs
	{
		void SCONE_API RegisterFactoryTypes()
		{
			// register sim factory types
			sim::RegisterFactoryTypes();

			// register opt factory types
			opt::RegisterFactoryTypes();

			// register objective
			opt::GetObjectiveFactory().Register< SimulationObjective >();

			// register controllers
			sim::GetControllerFactory().Register< FeedForwardController >();
			sim::GetControllerFactory().Register< GaitStateController >();
			sim::GetControllerFactory().Register< ReflexController >();
			sim::GetControllerFactory().Register< TimeStateController >();
			sim::GetControllerFactory().Register< MetaReflexController >();

			// register reflexes
			// TODO: make this not separate but use controller factory instead?
			GetReflexFactory().Register< MuscleReflex >();
			GetReflexFactory().Register< DofReflex >();
			GetReflexFactory().Register< ConditionalMuscleReflex >();

			// register measures
			sim::GetControllerFactory().Register< HeightMeasure >();
			sim::GetControllerFactory().Register< GaitMeasure >();
			sim::GetControllerFactory().Register< GaitCycleMeasure >();
			sim::GetControllerFactory().Register< EffortMeasure >();
			sim::GetControllerFactory().Register< DofLimitMeasure >();
			sim::GetControllerFactory().Register< CompositeMeasure >();
			sim::GetControllerFactory().Register< JumpMeasure >();
			sim::GetControllerFactory().Register< JointLoadMeasure >();
			sim::GetControllerFactory().Register< PointMeasure >();

			// register functions
			GetFunctionFactory().Register< PieceWiseConstantFunction >();
			GetFunctionFactory().Register< PieceWiseLinearFunction >();
			GetFunctionFactory().Register< Polynomial >();
		}

		PropNode SCONE_API RunSimulation( const String& par_file, bool write_results )
		{
			// create the simulation objective object
			cs::SimulationObjectiveUP so = CreateSimulationObjective( par_file );

			timer t;
			double result = so->Evaluate();
			auto duration = t.seconds();

			// set data storage
			if ( write_results )
				so->GetModel().SetStoreData( true );

			// reset profiler (only if enabled)
			Profiler::GetGlobalInstance().Reset();

			// collect statistics
			PropNode statistics;
			statistics.set( "result", result );
			statistics.get_child( "result" ).insert_children( so->GetMeasure().GetReport() );
			statistics.set( "simulation time", so->GetModel().GetTime() );
			statistics.set( "performance (x real-time)", so->GetModel().GetTime() / duration );
	
			// output profiler results (only if enabled)
			std::cout << Profiler::GetGlobalInstance().GetReport();

			// write results
			if ( write_results )
				so->WriteResults( bfs::path( par_file ).replace_extension().string() );

			return statistics;
		}

		SimulationObjectiveUP SCONE_API CreateSimulationObjective( const String& par_file )
		{
			opt::ParamSet par( par_file );
			bfs::path config_path = bfs::path( par_file ).parent_path() / "config.xml";
			if ( config_path.has_parent_path() )
				bfs::current_path( config_path.parent_path() );

			// read properties
			PropNode configProp = flut::load_xml( config_path.string() ) ;
			PropNode objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

			// create SimulationObjective object
			cs::SimulationObjectiveUP so = dynamic_unique_cast< cs::SimulationObjective >( opt::CreateObjective( objProp, par ) );

			// report unused parameters
			LogUntouched( objProp );

			return so;
		}
	}
}
