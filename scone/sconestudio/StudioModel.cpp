#include "StudioModel.h"

#include "scone/opt/Factories.h"
#include "scone/opt/opt_tools.h"
#include "scone/core/StorageIo.h"
#include "scone/cs/cs_tools.h"
#include "scone/core/system_tools.h"

#include <boost/filesystem.hpp>
namespace bfs = boost::filesystem;

namespace scone
{
	StudioModel::StudioModel( vis::scene &s, const String& par_file )
	{
		InitModel( par_file );
		InitVis( s );
	}

	void StudioModel::InitModel( const String& par_file )
	{
		// create the objective form par file
		so = cs::CreateSimulationObjective( par_file );

		// accept filename and clear data
		filename = par_file;

		// see if we can load a matching .sto file
		auto sto_file = bfs::path( filename ).replace_extension( "sto" );
		if ( bfs::exists( sto_file ) )
		{
			log::info( "Reading ", sto_file.string() );
			ReadStorageSto( data, sto_file.string() );
		}
		else
		{
			so->GetModel().SetStoreData( true );
			so->Evaluate();
			log::info( so->GetMeasure().GetReport() );
			so->WriteResults( flut::get_filename_without_ext( filename ) );
			data = so->GetModel().GetData();
		}

		// setup state_data_index
		auto state_names = so->GetModel().GetStateVariableNames();
		state_data_index.resize( state_names.size() );
		for ( size_t state_idx = 0; state_idx < state_data_index.size(); state_idx++ )
		{
			auto data_idx = data.GetChannelIndex( state_names[ state_idx ] );
			SCONE_ASSERT_MSG( data_idx != NoIndex, "Could not find state channel " + state_names[ state_idx ] );
			state_data_index[ state_idx ] = data_idx;
		}
	}

	void StudioModel::InitVis( vis::scene& s )
	{
		sim::Model& model = so->GetModel();

		com = s.make_sphere( 0.1f, vis::make_red(), 0.9f );

		for ( auto& body : model.GetBodies() )
		{
			if ( body->HasVisualizationGeometry() )
			{
				log::info( "Geometry for body ", body->GetName(), ": ", body->GetVisualizationFilename() );
				bodies.push_back( scone::GetFolder( scone::SCONE_GEOMETRY_FOLDER ) + body->GetVisualizationFilename() );
			}
			else
			{
				bodies.push_back( s.make_cube( vis::vec3f( 0.1f, 0.1f, 0.1f ), vis::make_cyan( 0.5 ) ) );
			}
		}

		bodies[ 0 ].show( false );
	}

	void StudioModel::UpdateVis( TimeInSeconds time )
	{
		sim::Model& model = so->GetModel();

		// update model state from data
		std::vector< Real > state( state_data_index.size() );
		for ( Index i = 0; i < state.size(); ++i )
			state[ i ] = data.GetInterpolatedValue( time, state_data_index[ i ] );
		model.SetStateValues( state );

		// update objects		
		com.pos( model.GetComPos() );

		auto& model_bodies = model.GetBodies();
		for ( Index i = 0; i < model_bodies.size(); ++i )
		{
			auto bp = model_bodies[ i ]->GetOrigin();
			bodies[ i ].pos_ori( model_bodies[ i ]->GetOrigin(), model_bodies[ i ]->GetOri() );
		}
	}

	void StudioModel::EvaluateObjective()
	{
	}
}
