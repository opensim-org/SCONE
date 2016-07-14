#include "StudioModel.h"

#include "scone/opt/Factories.h"
#include "scone/opt/opt_tools.h"
#include "scone/core/StorageIo.h"
#include "scone/cs/cs_tools.h"
#include "scone/core/system_tools.h"
#include "scone/sim/Muscle.h"

#include <boost/filesystem.hpp>
#include "flut/timer.hpp"

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
			flut::timer t;
			log::info( "Reading ", sto_file.string() );
			ReadStorageSto( data, sto_file.string() );
			log::trace( "File read in ", t.seconds(), " seconds" );
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

		//com = s.add_sphere( 0.1f, vis::make_red(), 0.9f );

		flut::timer t;
		for ( auto& body : model.GetBodies() )
		{
			body_meshes.push_back( std::vector< vis::mesh >() );
			auto geom_files = body->GetDisplayGeomFileNames();

			for ( auto& geom_file : geom_files )
			{
				//log::debug( "Loading geometry for body ", body->GetName(), ": ", geom_file );
				body_meshes.back().push_back( s.add_mesh( scone::GetFolder( scone::SCONE_GEOMETRY_FOLDER ) + geom_file ) );
				body_meshes.back().back().set_color( vis::color( 1, 0.98, 0.95 ), 1, 15, 0, 0.5f );
			}
		}
		log::trace( "Meshes loaded in ", t.seconds(), " seconds" );

		for ( auto& muscle : model.GetMuscles() )
		{
			// add path
			auto p = muscle->GetMusclePath();
			muscles.push_back( vis::path( s, p.size(), 0.01f, vis::make_red(), 0.3f ) );
		}
	}

	void StudioModel::UpdateVis( TimeInSeconds time )
	{
		sim::Model& model = so->GetModel();

		// update model state from data
		std::vector< Real > state( state_data_index.size() );
		for ( Index i = 0; i < state.size(); ++i )
			state[ i ] = data.GetInterpolatedValue( time, state_data_index[ i ] );
		model.SetStateValues( state );

		// update com
		//com.pos( model.GetComPos() );

		// update bodies
		auto& model_bodies = model.GetBodies();
		for ( Index i = 0; i < model_bodies.size(); ++i )
		{
			auto bp = model_bodies[ i ]->GetOrigin();
			for ( auto& bm : body_meshes[ i ] )
				bm.pos_ori( model_bodies[ i ]->GetOrigin(), model_bodies[ i ]->GetOri() );
		}

		// update muscle paths
		auto &model_muscles = model.GetMuscles();
		for ( Index i = 0; i < model_muscles.size(); ++i )
		{
			auto mp = model_muscles[ i ]->GetMusclePath();
			muscles[ i ].set_points( mp );

			auto a = model_muscles[ i ]->GetActivation();
			muscles[ i ].set_color( vis::color( a, 0, 0.5 - 0.5 * a, 1 ), 0.5f, 15, 0, 0.5f );
		}
	}

	void StudioModel::EvaluateObjective()
	{
	}
}
