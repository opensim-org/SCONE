#pragma once

#include "scone/core/types.h"
#include "scone/controllers/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/core/Function.h"
#include "scone/model/Leg.h"

namespace OpenSim
{
	class PiecewiseLinearFunction;
}

namespace scone
{
	class SCONE_API FeedForwardController : public Controller
	{
	public:
		FeedForwardController( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~FeedForwardController() { };

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		bool UseModes() { return number_of_modes > 0; }

		// a signature describing the controller
		virtual String GetClassSignature() const override;

	private:
		// mode settings
		size_t number_of_modes;

		// muscle info
		struct ActInfo
		{
			ActInfo() : side( NoSide ), function_idx( NoIndex ), muscle_idx( NoIndex ) {};
			String name;
			Side side;
			String full_name;
			size_t function_idx;
			size_t muscle_idx;
			std::vector< double > mode_weights;
		};

		std::vector< FunctionUP > m_Functions;
		std::vector< ActInfo > m_ActInfos;
		bool symmetric;

	private: // non-copyable and non-assignable
		FeedForwardController( const FeedForwardController& );
		FeedForwardController& operator=( const FeedForwardController& );
	};
}
