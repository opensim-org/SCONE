/*
** ImitationObjective.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "ImitationObjective.h"

#include "scone/core/Exception.h"
#include "scone/model/Model.h"
#include "scone/core/version.h"
#include "scone/core/string_tools.h"
#include "scone/core/system_tools.h"
#include "scone/core/Factories.h"
#include "scone/core/StorageIo.h"
#include "scone/model/Muscle.h"
#include "scone/core/profiler_config.h"

#include <vector>

#include "xo/container/prop_node.h"
#include "xo/container/container_tools.h"

namespace scone
{
	ImitationObjective::ImitationObjective( const PropNode& pn, const path& find_file_folder ) :
	ModelObjective( pn, find_file_folder )
	{
		file = FindFile( pn.get<path>( "file" ) );
		INIT_PROP( pn, frame_delta, 1 );

		if ( signature_postfix.empty() )
			signature_postfix = "Imitation";

		// prepare data
		ReadStorageSto( m_Storage, file );

		// make sure data and model are compatible
		auto state = model_->GetState();
		SCONE_THROW_IF( state.GetSize() > m_Storage.GetChannelCount(), "File and model are incompatible for ImitationObjective" );
		for ( index_t i = 0; i < state.GetSize(); ++i )
			SCONE_THROW_IF( state.GetName( i ) != m_Storage.GetLabels()[ i ], "File and model are incompatible for ImitationObjective" );

		// find excitation channels
		m_ExcitationChannels.reserve( model_->GetMuscles().size() );
		for ( auto& mus : model_->GetMuscles() )
		{
			m_ExcitationChannels.push_back( m_Storage.GetChannelIndex( mus->GetName() + ".excitation" ) );
			SCONE_THROW_IF( m_ExcitationChannels.back() == NoIndex, "Could not find excitation for " + mus->GetName() );
		}

		// find sensor channels
		auto& ds = model_->GetSensorDelayStorage();
		m_SensorChannels.reserve( ds.GetChannelCount() );
		for ( index_t ds_idx = 0; ds_idx < ds.GetChannelCount(); ++ds_idx )
		{
			auto& sensor_name = ds.GetLabels()[ ds_idx ];
			m_SensorChannels.push_back( m_Storage.GetChannelIndex( sensor_name ) );
			SCONE_THROW_IF( m_SensorChannels.back() == NoIndex, "Could not find sensor for " + sensor_name );
		}
	}

	ImitationObjective::~ImitationObjective()
	{}

	void ImitationObjective::AdvanceSimulationTo( Model& model, TimeInSeconds t ) const
	{
		SCONE_PROFILE_FUNCTION;

		if ( !model.GetUserData().has_key( "IM_fra" ) )
		{
			SCONE_PROFILE_SCOPE( "SetupStorage" );
			model.GetUserData()[ "IM_fra" ] = 0;
			model.GetUserData()[ "IM_res" ] = 0.0;

			// add sensor data
			auto& ds = model.GetSensorDelayStorage();
			for ( index_t fidx = 1; fidx < m_Storage.GetFrameCount(); ++fidx )
			{
				auto sf = m_Storage.GetFrame( fidx );
				ds.AddFrame( sf.GetTime() );
				for ( index_t cidx = 0; cidx < m_SensorChannels.size(); ++cidx )
					ds.Back()[ cidx ] = sf[ m_SensorChannels[ cidx ] ];
			}
		}

		// compute result
		double result = 0.0;
		index_t frame_start = model.GetUserData().get< index_t >( "IM_fra" );
		index_t frame_count = 0;

		{
			SCONE_PROFILE_SCOPE( "ComputeSimularity" );
			for ( index_t idx = frame_start * frame_delta; idx < m_Storage.GetFrameCount() && m_Storage.GetFrame( idx ).GetTime() <= t; idx += frame_delta )
			{
				auto f = m_Storage.GetFrame( idx );

				// set state and compare output
				model.SetStateValues( f.GetValues(), f.GetTime() );
				for ( index_t idx = 0; idx < m_ExcitationChannels.size(); ++idx )
					result += abs( model.GetMuscles()[ idx ]->GetExcitation() - f[ m_ExcitationChannels[ idx ] ] );
				++frame_count;
			}
		}

		if ( frame_count > 0 )
		{
			result = 100 * result / m_ExcitationChannels.size();
			//log::trace( "t=", t, " frames=", frame_count, " start=", frame_start, " result=", result );
			model.GetUserData()[ "IM_res" ] = result + model.GetUserData().get< double >( "IM_res" );
			model.GetUserData()[ "IM_fra" ] = frame_start + frame_count;
		}
	}

	TimeInSeconds ImitationObjective::GetDuration() const
	{
		// find last frame, keeping frame_delta in mind
		auto lastFrame = ( m_Storage.GetFrameCount() - 1 ) / frame_delta * frame_delta;
		return m_Storage.GetFrame( lastFrame ).GetTime();
	}

	fitness_t ImitationObjective::GetResult( Model& m ) const
	{
		return m.GetUserData().get< fitness_t >( "IM_res" ) / ( m.GetUserData().get< index_t >( "IM_fra" ) );
	}

	PropNode ImitationObjective::GetReport( Model& m ) const
	{
		return xo::to_prop_node( GetResult( m ) );
	}
}
