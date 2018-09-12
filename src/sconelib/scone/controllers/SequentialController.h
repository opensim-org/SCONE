#pragma once

#include "CompositeController.h"

namespace scone
{
	class SequentialController : public CompositeController
	{
	public:
		SequentialController( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~SequentialController() {}

		virtual bool ComputeControls( Model& model, double timestamp ) override;

	protected:
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override;
		index_t GetActiveIdx( double timestamp );
		virtual String GetClassSignature() const override;

		std::vector< ControllerUP > controllers_;
		std::vector< double > transition_times_;
	};
}
