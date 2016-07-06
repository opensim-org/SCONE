#include "StudioModel.h"

#include "scone/cs/SimulationObjective.h"
#include "scone/opt/Factories.h"
#include "scone/opt/opt_tools.h"
#include "scone/core/StorageIo.h"

#include <boost/filesystem.hpp>

namespace bfs = boost::filesystem;

namespace scone
{
	StudioModel::StudioModel( vis::scene &s, const String& par_file )
	{
		CreateObjective( par_file );
		InitVis( s );
	}

	void StudioModel::CreateObjective( const String& par_file )
	{
		opt::ParamSet par( par_file );
		SCONE_ASSERT_MSG( !par.Empty(), "Could not open file " + par_file );

		// accept filename and clear data
		filename = par_file;
		data.Clear();

		// get config path and name
		auto config_path = bfs::path( filename ).parent_path() / "config.xml";
		if ( config_path.has_parent_path() )
			bfs::current_path( config_path.parent_path() );
	
		PropNode configProp = ReadPropNodeFromXml( config_path.string() ) ;
		const PropNode& objProp = configProp.GetChild( "Optimizer.Objective" );

		// create objective
		opt::ObjectiveUP obj = opt::CreateObjective( objProp, par );
		objective = opt::CreateObjective( objProp, par );
		cs::SimulationObjective& so = dynamic_cast< cs::SimulationObjective& >( *objective );

		// report unused parameters
		opt::LogUntouched( objProp );

		// see if we can load a matching .sto file
		auto stofile = bfs::path( filename ).replace_extension( "sto" );
		if ( bfs::exists( stofile ) )
		{
			log::debug( "Reading ", stofile.string() );
			ReadStorageSto( data, stofile.string() );
		}
	}

	void StudioModel::InitVis( vis::scene& s )
	{

	}

	void StudioModel::Update( TimeInSeconds t )
	{

	}

	void StudioModel::EvaluateObjective()
	{

	}


}
