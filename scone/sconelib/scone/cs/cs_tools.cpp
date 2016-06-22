#include "cs_tools.h"
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

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

#include <flut/timer.hpp>
using flut::timer;

namespace scone
{
	namespace cs
	{
		void SCONE_API RegisterFactoryTypes()
		{
			// register sim factory types
			sim::RegisterFactoryTypes();

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

			// register functions
			GetFunctionFactory().Register< PieceWiseConstantFunction >();
			GetFunctionFactory().Register< PieceWiseLinearFunction >();
			GetFunctionFactory().Register< Polynomial >();
		}

		PropNode SCONE_API RunSimulation( const String& par_file, bool write_results /*= false */ )
		{
			opt::RegisterFactoryTypes();
			cs::RegisterFactoryTypes();
			sim::RegisterSimbody();

			opt::ParamSet par( par_file );

			bfs::path config_path = bfs::path( par_file ).parent_path() / "config.xml";
			if ( config_path.has_parent_path() )
				bfs::current_path( config_path.parent_path() );

			PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
			PropNode objProp = configProp.GetChild( "Optimizer.Objective" );

			// create objective
			opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
			cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

			double result;
			timer t;
			result = obj->Evaluate();
			auto duration = t.seconds();

			// collect statistics
			PropNode stats;
			stats.Clear();
			stats.Set( "result", result );
			stats.AddChild( "report", so.GetMeasure().GetReport() );
			stats.Set( "simulation time", so.GetModel().GetTime() );
			stats.Set( "performance (x real-time)", so.GetModel().GetTime() / duration );

			// write results
			if ( write_results )
				obj->WriteResults( bfs::path( par_file ).replace_extension().string() );

			return stats;
		}
	}
}
