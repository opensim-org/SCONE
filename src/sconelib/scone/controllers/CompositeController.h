#pragma once

#include "../model/Controller.h"

namespace scone
{
	class CompositeController : public Controller
	{
	public:
		CompositeController( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~CompositeController() {}
		
		virtual bool UpdateControls( Model& model, double timestamp ) override;
		virtual bool UpdateAnalysis( const Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual void WriteResult( const path& file ) const override;

	protected:
		virtual String GetClassSignature() const override;

		std::vector< ControllerUP > m_Controllers;
	};
}
