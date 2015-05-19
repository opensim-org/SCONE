#include "stdafx.h"

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

#include "SconeManager.h"
#include "../opt/opt.h"
#include "../cs/cs.h"
#include "../core/system.h"
#include "../opt/ParamSet.h"
#include "../sim/Factories.h"
#include "../opt/Factories.h"
#include "../core/Timer.h"
#include "../cs/SimulationObjective.h"
#include "../core/Log.h"
#include "../core/Profiler.h"

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
		
		Profiler::GetGlobalInstance().Reset();

		m_Statistics.Clear();
		Timer timer;
		double result = so.Evaluate();
		timer.Pause();
	
		// collect statistics
		m_Statistics.Clear();
		m_Statistics.Set( "result", result );
		m_Statistics.GetChild( "result" ).InsertChildren( so.GetMeasure().GetReport() );
		m_Statistics.Set( "simulation time", so.GetModel().GetTime() );
		m_Statistics.Set( "performance (x real-time)", so.GetModel().GetTime() / timer.GetTime() );
	
		cout << "--- Evaluation report ---" << endl;
		cout << m_Statistics << endl;

		cout << Profiler::GetGlobalInstance().GetReport();

		// write results
		obj->WriteResults( path( filename ).replace_extension().string() );
	}
}
