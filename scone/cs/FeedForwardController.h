#pragma once

#include "cs.h"
#include "scone/sim/Controller.h"
#include "scone/core/PropNode.h"
#include "scone/opt/ParamSet.h"
#include "Function.h"
#include "scone/sim/Leg.h"

namespace OpenSim
{
	class PiecewiseLinearFunction;
}

namespace scone
{
	namespace cs
	{
		class CS_API FeedForwardController : public sim::Controller
		{
		public:
			FeedForwardController( const PropNode& props, opt::ParamSet& par, sim::Model& model, const sim::Area& target_area );
			virtual ~FeedForwardController() { };

			virtual UpdateResult UpdateControls( sim::Model& model, double timestamp ) override;
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

		private: // non-copyable and non-assignable
			FeedForwardController( const FeedForwardController& );
			FeedForwardController& operator=( const FeedForwardController& );
		};
	}
}
