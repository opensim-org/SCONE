#pragma once

#include "scone/controllers/Controller.h"

namespace scone
{
	class CompositeController : public Controller
	{
	public:
		CompositeController( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~CompositeController() {}
		
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual std::vector<xo::path> WriteResults( const xo::path& file ) const override;

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override;

		virtual String GetClassSignature() const override;
		std::vector< ControllerUP > controllers_;
	};
}
