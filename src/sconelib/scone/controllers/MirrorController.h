#pragma once

#include "scone/model/Controller.h"
#include "scone/core/Factories.h"

namespace scone
{
	class MirrorController : public Controller
	{
	public:
		MirrorController( const PropNode& props, Params& par, Model& model, const Locality& area ) :
		Controller( props, par, model, area )
		{
			for ( auto& pn : props )
			{
				if ( pn.first == "Controller" )
				{
					c0 = CreateController( pn.second, par, model, area );
					c1 = CreateController( pn.second, par, model, MakeMirrored( area ) );
				}
			}
		}
		virtual ~MirrorController() {}

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override
		{
			c0->StoreData( frame, flags );
			c1->StoreData( frame, flags );
		}

		virtual bool UpdateAnalysis( const Model& model, double timestamp ) override
		{
			c0->UpdateAnalysis( model, timestamp );
			return c1->UpdateAnalysis( model, timestamp );
		}


		virtual bool UpdateControls( Model& model, double timestamp ) override
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
