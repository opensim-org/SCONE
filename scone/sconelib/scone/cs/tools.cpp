#include "tools.h"

#include "scone/core/core.h"
#include "scone/core/system_tools.h"
#include "scone/core/Log.h"

#include "scone/opt/opt.h"
#include "scone/opt/ParamSet.h"
#include "scone/opt/Factories.h"

#include "scone/cs/cs.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/sim/simbody/sim_simbody.h"

#include <boost/filesystem.hpp>

#include <flut/timer.hpp>
using flut::timer;

namespace bfs = boost::filesystem;
using std::cout;
using std::endl;

namespace scone
{
	scone::PropNode RunSimulation( const String& par_file, bool write_results /*= false */ )
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
