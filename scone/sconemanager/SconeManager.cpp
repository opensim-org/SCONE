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

	SconeManager::~SconeManager()
	{
	}

	void SconeManager::SimulateObjective( const String& filename )
	{
		cout << "--- Starting evaluation ---" << endl;
	
		opt::ParamSet par( filename );
	
		path config_path = path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			current_path( config_path.parent_path() );
	
		PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
		const PropNode& objProp = configProp.GetChild( "Optimizer.Objective" );
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );

		// report unused parameters
		LogUntouched( objProp );

		// set data storage
		so.GetModel().SetStoreData( true );
		
		Profiler::GetGlobalInstance().Reset();

		m_Statistics.Clear();
		timer tmr;
		double result = obj->Evaluate();
		auto duration = tmr.seconds();
	
		// collect statistics
		m_Statistics.Clear();
		m_Statistics.Set( "result", result );
		m_Statistics.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
		m_Statistics.Set( "simulation time", so.GetModel().GetTime() );
		m_Statistics.Set( "performance (x real-time)", so.GetModel().GetTime() / duration );
	
		cout << "--- Evaluation report ---" << endl;
		cout << m_Statistics << endl;

		cout << Profiler::GetGlobalInstance().GetReport();

		// write results
		obj->WriteResults( path( filename ).replace_extension().string() );
	}
}
