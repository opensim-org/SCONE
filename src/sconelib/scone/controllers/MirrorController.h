#pragma once

#include "scone/controllers/Controller.h"
#include "scone/core/Factories.h"

namespace scone
{
	/// Controller that automatically generates a mirrored version of a child Controller.
	class MirrorController : public Controller
	{
	public:
		MirrorController( const PropNode& props, Params& par, Model& model, const Location& loc ) :
		Controller( props, par, model, loc )
		{
			for ( auto& pn : props )
			{
				if ( pn.first == "Controller" )
				{
					c0 = CreateController( pn.second, par, model, Location( RightSide, loc.symmetric ) );
					c1 = CreateController( pn.second, par, model, Location( LeftSide, loc.symmetric ) );
				}
			}
		}
		virtual ~MirrorController() {}

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override
		{
			c0->StoreData( frame, flags );
			c1->StoreData( frame, flags );
		}

		virtual bool PerformAnalysis( const Model& model, double timestamp ) override
		{
			c0->UpdateAnalysis( model, timestamp );
			return c1->UpdateAnalysis( model, timestamp );
		}


		virtual bool ComputeControls( Model& model, double timestamp ) override
		{
			c0->UpdateControls( model, timestamp );
			return c1->UpdateControls( model, timestamp );
		}

	protected:
		virtual String GetClassSignature() const override
		{
			return c0->GetSignature();
		}

	private:
		ControllerUP c0, c1;
	};
}
