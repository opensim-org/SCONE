#include "CompositeController.h"

#include "scone/core/Factories.h"
#include "xo/container/container_tools.h"

namespace scone
{
	CompositeController::CompositeController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
	Controller( props, par, model, loc )
	{
		for ( auto& cpn : props.select( "Controller" ) )
			controllers_.emplace_back( CreateController( cpn.second, par, model, loc ) );
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
		String s = "C";
		for ( auto& c : controllers_ )
			s += '.' + c->GetSignature();
		return s;
	}
}
