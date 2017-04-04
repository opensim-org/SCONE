#include "cs_tools.h"
#include "scone/core/memory_tools.h"
#include "scone/core/PieceWiseConstantFunction.h"
#include "scone/core/PieceWiseLinearFunction.h"
#include "scone/core/Polynomial.h"
#include "scone/core/Profiler.h"
#include "scone/controllers/ConditionalMuscleReflex.h"
#include "scone/controllers/DofReflex.h"
#include "scone/controllers/Factories.h"
#include "scone/controllers/FeedForwardController.h"
#include "scone/controllers/GaitStateController.h"
#include "scone/controllers/MetaReflexController.h"
#include "scone/controllers/MuscleReflex.h"
#include "scone/controllers/ReflexController.h"
#include "scone/controllers/TimeStateController.h"
#include "scone/objectives/CompositeMeasure.h"
#include "scone/objectives/DofLimitMeasure.h"
#include "scone/objectives/EffortMeasure.h"
#include "scone/objectives/GaitCycleMeasure.h"
#include "scone/objectives/GaitMeasure.h"
#include "scone/objectives/HeightMeasure.h"
#include "scone/objectives/JointLoadMeasure.h"
#include "scone/objectives/JumpMeasure.h"
#include "scone/objectives/PointMeasure.h"
#include "scone/objectives/ReactionForceMeasure.h"
#include "scone/objectives/SimulationObjective.h"
#include "scone/optimization/Factories.h"
#include "scone/optimization/Optimizer.h"
#include "scone/optimization/opt_tools.h"
#include "scone/model/Factories.h"
#include "scone/model/sim_tools.h"
#include "scone/model/simbody/sim_simbody.h"

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <flut/timer.hpp>
#include "flut/prop_node_tools.hpp"
#include "PerturbationController.h"
using flut::timer;

namespace scone
{
	void SCONE_API RegisterFactoryTypes()
	{
		// register sim factory types
		sim::RegisterFactoryTypes();

		// register opt factory types
		opt::RegisterFactoryTypes();

		// register objective
		opt::GetObjectiveFactory().Register< SimulationObjective >();

		// register reflexes
		// TODO: make this not separate but use controller factory instead?
		GetReflexFactory().Register< MuscleReflex >();
		GetReflexFactory().Register< DofReflex >();
		GetReflexFactory().Register< ConditionalMuscleReflex >();

		// register functions
		GetFunctionFactory().Register< PieceWiseConstantFunction >();
		GetFunctionFactory().Register< PieceWiseLinearFunction >();
		GetFunctionFactory().Register< Polynomial >();
	}

	PropNode SCONE_API RunSimulation( const path& par_file, bool write_results )
	{
		// create the simulation objective object
		SimulationObjectiveUP so = CreateSimulationObjective( par_file );

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
		statistics.set( "result", so->GetMeasure().GetReport() );
		statistics.set( "simulation time", so->GetModel().GetTime() );
		statistics.set( "performance (x real-time)", so->GetModel().GetTime() / duration );

		// output profiler results (only if enabled)
		std::cout << Profiler::GetGlobalInstance().GetReport();

		// write results
		if ( write_results )
			so->WriteResults( bfs::path( par_file.str() ).replace_extension().string() );

		return statistics;
	}

	SimulationObjectiveUP SCONE_API CreateSimulationObjective( const path& filename )
	{
		opt::ParamSet par;
		bfs::path config_path;
		if ( filename.extension() == "par" )
		{
			par.Read( filename );
			config_path = bfs::path( filename.str() ).parent_path() / "config.xml";
		}
		else config_path = filename.str();
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );

		// read properties
		PropNode configProp = flut::load_file_with_include( config_path.string(), "INCLUDE" );
		PropNode objProp = configProp.get_child( "Optimizer" ).get_child( "Objective" );

		// load init parameters if we just load a config file
		if ( filename.extension() == "xml" || filename.extension() == "scenario" )
		{
			auto& optProp = configProp.get_child( "Optimizer" );
			if ( optProp.get< bool >( "use_init_file" ) )
				par.Read( optProp.get< path >( "init_file" ) );
		}

		// create SimulationObjective object
		SimulationObjectiveUP so = dynamic_unique_cast<SimulationObjective>( opt::CreateObjective( objProp, par ) );

		// report unused parameters
		LogUntouched( objProp );

		return so;
	}
}
