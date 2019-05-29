/*
** CompositeController.cpp
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "CompositeController.h"

#include "scone/core/Factories.h"
#include "xo/container/container_tools.h"
#include "scone/core/Log.h"

namespace scone
{
	CompositeController::CompositeController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc )
	{
		for ( auto& cpn : props )
			if ( auto fp = MakeFactoryProps( GetControllerFactory(), cpn, "Controller" ) )
				controllers_.emplace_back( CreateController( fp, par, model, loc ) );

		if ( Controllers = props.try_get_child( "Controllers" ) )
			for ( auto& cpn : *Controllers )
				if ( auto fp = MakeFactoryProps( GetControllerFactory(), cpn, "Controller" ) )
					controllers_.emplace_back( CreateController( fp, par, model, loc ) );

		// display warning if child controllers have identical names
		if ( controllers_.size() > 1 )
		{
			for ( int i1 = 0; i1 < controllers_.size() - 1; ++i1 )
				for ( int i2 = i1 + 1; i2 < controllers_.size(); ++i2 )
					if ( controllers_[ i1 ]->GetName() == controllers_[ i2 ]->GetName() )
						log::warning( "Warning: child controllers ", i1, " and ", i2, " have identical names, parameters may get mixed up" );
		}
	}

	bool CompositeController::ComputeControls( Model& model, double timestamp )
	{
		bool terminate = false;
		for ( auto& c : controllers_ )
			terminate |= c->UpdateControls( model, timestamp );
		return terminate;
	}

	bool CompositeController::PerformAnalysis( const Model& model, double timestamp )
	{
		bool terminate = false;
		for ( auto& c : controllers_ )
			terminate |= c->UpdateAnalysis( model, timestamp );
		return terminate;
	}

	void CompositeController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& c : controllers_ )
			c->StoreData( frame, flags );
	}

	std::vector<xo::path> CompositeController::WriteResults( const xo::path& file ) const
	{
		std::vector<xo::path> files;
		for ( auto& c : controllers_ )
			xo::append( files, c->WriteResults( file ) );
		return files;
	}

	String CompositeController::GetClassSignature() const
	{
		std::vector< String > strset;
		for ( auto& c : controllers_ )
		{
			string s = c->GetSignature();
			if ( xo::find( strset, s ) == strset.end() )
				strset.emplace_back( s );
		}
		return xo::container_to_str( strset, "." );
	}
}
