#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "SconeManager.h"
#include "scone/opt/opt.h"
#include "scone/cs/cs.h"
#include "scone/core/system.h"
#include "scone/opt/ParamSet.h"
#include "scone/sim/Factories.h"
#include "scone/opt/Factories.h"
#include "scone/core/tools.h"
#include "scone/cs/SimulationObjective.h"
#include "scone/core/Log.h"
#include "scone/core/Profiler.h"

#include "scone/sim/simbody/sim_simbody.h"
#include "scone/core/StorageIo.h"

using namespace boost::filesystem;
using boost::format;
using namespace std;

namespace scone
{
	SconeManager::SconeManager()
	{
		// set log level
		log::SetLevel( log::TraceLevel );

		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();
		sim::RegisterSimbody();
	}

	SconeManager::~SconeManager() { }

	void SconeManager::InitParFile( const String& f )
	{
		opt::ParamSet par( f );

		SCONE_ASSERT_MSG( !par.Empty(), "Could not open file " + f );

		// accept filename and clear data
		filename = f;
		data.Clear();

		// get config path and name
		path config_path = path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			current_path( config_path.parent_path() );
	
		PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
		const PropNode& objProp = configProp.GetChild( "Optimizer.Objective" );

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		objective = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *objective );

		// report unused parameters
		LogUntouched( objProp );

		// see if we can load a matching .sto file
		path stofile = path( filename ).replace_extension( "sto" );
		if ( exists( stofile ) )
			ReadStorageSto( data, stofile.string() );
	}

	void SconeManager::Evaluate( TimeInSeconds endtime /*= 0.0 */ )
	{
		SCONE_ASSERT_MSG( objective, "No objective was loaded" );

		log::info( "--- Starting evaluation ---" );

		// set data storage
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *objective );
		so.GetModel().SetStoreData( true );

		// init profiler
		Profiler::GetGlobalInstance().Reset();

		// prepare stats
		statistics.Clear();
		timer tmr;


		double result = so.Evaluate();
		auto duration = tmr.seconds();
	
		// collect statistics
		statistics.Clear();
		statistics.Set( "result", result );
		statistics.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
		statistics.Set( "simulation time", so.GetModel().GetTime() );
		statistics.Set( "performance (x real-time)", so.GetModel().GetTime() / duration );

		// log some stuff
		log::info( "--- Evaluation report ---" );
		log::info( statistics );
		log::info( Profiler::GetGlobalInstance().GetReport() );

		// copy data and write results
		so.WriteResults( path( filename ).replace_extension().string() );
		data = so.GetModel().GetData();
	}
}
