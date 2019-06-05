#include "MirrorController.h"

namespace scone
{
	MirrorController::MirrorController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
		Controller( props, par, model, loc )
	{
		for ( auto& pn : props )
		{
			if ( auto fp = MakeFactoryProps( GetControllerFactory(), pn, "Controller" ) )
			{
				c0 = CreateController( fp, par, model, Location( RightSide, loc.symmetric_ ) );
				c1 = CreateController( fp, par, model, Location( LeftSide, loc.symmetric_ ) );
			}
		}
	}

	MirrorController::~MirrorController()
	{}

	void MirrorController::StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const
	{
		c0->StoreData( frame, flags );
		c1->StoreData( frame, flags );
	}

	bool MirrorController::PerformAnalysis( const Model& model, double timestamp )
	{
		bool b = c0->UpdateAnalysis( model, timestamp );
		b |= c1->UpdateAnalysis( model, timestamp );
		return b;
	}

	bool MirrorController::ComputeControls( Model& model, double timestamp )
	{
		bool b = c0->UpdateControls( model, timestamp );
		b |= c1->UpdateControls( model, timestamp );
		return b;
	}

	String MirrorController::GetClassSignature() const
	{
		return c0->GetSignature();
	}

}

