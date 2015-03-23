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

using namespace boost::filesystem;
using boost::format;
using namespace std;

namespace scone
{
	SconeManager::SconeManager()
	{
		// register scone types
		opt::RegisterFactoryTypes();
		cs::RegisterFactoryTypes();
	}

	SconeManager::~SconeManager()
	{
	}

	const PropNode& SconeManager::GetSettings()
	{
		// lazy initialization
		if ( m_Settings.IsEmpty() )
			m_Settings.FromIniFile( GetLocalAppDataFolder() + "/Scone/settings.ini" );

		return m_Settings;
	}

	void SconeManager::SimulateObjective( const String& filename )
	{
		cout << "--- Starting evaluation ---" << endl;
	
		opt::ParamSet par( filename );
	
		path config_path = path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			current_path( config_path.parent_path() );
	
		PropNode configProp = ReadPropertiesFromXml( config_path.string() ) ;
		const PropNode& objProp = configProp.GetChild( "Optimizer.Objective" );
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *obj );
		
		m_Statistics.Clear();
		Timer timer;
		double result = so.Evaluate();
		timer.Pause();
	
		// collect statistics
		m_Statistics.Clear();
		m_Statistics.Set( "result", result );
		m_Statistics.GetChild( "result" ).Merge( so.GetMeasure().GetReport(), false );
		m_Statistics.Set( "simulation time", so.GetModel().GetTime() );
		m_Statistics.Set( "performance (x real-time)", so.GetModel().GetTime() / timer.GetTime() );
	
		cout << "--- Evaluation report ---" << endl;
		cout << m_Statistics << endl;
	}
}
