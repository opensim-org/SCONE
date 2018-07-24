#pragma once

#include "scone/core/types.h"
#include "scone/core/HasSignature.h"
#include "scone/core/HasData.h"
#include "scone/model/State.h"
#include "scone/optimization/Params.h"

namespace scone
{
	class SCONE_API Controller : public HasSignature, public HasData
	{
	public:
		Controller( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~Controller();

		/// Called each attempted integration step, returns true on termination request
		virtual bool UpdateControls( Model& model, double timestamp ) { return false; }

		/// Called after each successful integration step, returns true on termination request
		virtual bool UpdateAnalysis( const Model& model, double timestamp ) { return false; }

		// default implementation doesn't store anything
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}

		// default implementation doesn't store anything
		virtual void WriteResult( const path& file ) const {}

	private:
		bool m_TerminationRequest;
	};
}
