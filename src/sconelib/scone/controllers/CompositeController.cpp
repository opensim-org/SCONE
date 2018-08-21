#include "CompositeController.h"

#include "scone/core/Factories.h"

namespace scone
{
	CompositeController::CompositeController( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Controller( props, par, model, area )
	{
		for ( auto& cpn : props.select( "Controller" ) )
			controllers_.emplace_back( CreateController( cpn.second, par, model, area ) );
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

	void CompositeController::WriteResult( const xo::path& file ) const
	{
		for ( auto& c : controllers_ )
			c->WriteResult( file );
	}

	String CompositeController::GetClassSignature() const
	{
		String s = "C";
		for ( auto& c : controllers_ )
			s += '.' + c->GetSignature();
		return s;
	}
}
