#include "CompositeController.h"

#include "../core/Factories.h"

namespace scone
{
	
	CompositeController::CompositeController( const PropNode& props, Params& par, Model& model, const Locality& area ) :
	Controller( props, par, model, area )
	{
		for ( auto& cpn : props.select( "Controller" ) )
			m_Controllers.emplace_back( CreateController( cpn.second, par, model, area ) );
	}

	bool CompositeController::UpdateControls( Model& model, double timestamp )
	{
		bool terminate = false;
		for ( auto& c : m_Controllers )
			terminate |= c->UpdateControls( model, timestamp );
		return terminate;
	}

	bool CompositeController::UpdateAnalysis( const Model& model, double timestamp )
	{
		bool terminate = false;
		for ( auto& c : m_Controllers )
			terminate |= c->UpdateAnalysis( model, timestamp );
		return terminate;
	}

	void CompositeController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		for ( auto& c : m_Controllers )
			c->StoreData( frame, flags );
	}

	void CompositeController::WriteResult( const path& file ) const
	{
		for ( auto& c : m_Controllers )
			c->WriteResult( file );
	}

	String CompositeController::GetClassSignature() const
	{
		String s = "C";
		for ( auto& c : m_Controllers )
			s += '.' + c->GetSignature();
		return s;
	}
}
