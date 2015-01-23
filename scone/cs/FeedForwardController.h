#pragma once
#include "../sim/Controller.h"
#include "cs.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"

namespace OpenSim
{
	class PiecewiseLinearFunction;
}

namespace scone
{
	namespace cs
	{
		class CS_API FeedForwardController : public sim::Controller, public Factoryable< sim::Controller, FeedForwardController >
		{
		public:
			FeedForwardController( const PropNode& props );
			virtual ~FeedForwardController() { };

			virtual void ProcessParameters( opt::ParamSet& par ) override;
			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual void Initialize( sim::Model& model ) override;

		private:
			String function_type;
			bool use_symmetric_actuators;
			size_t control_points;
			double control_point_time_delta;
			double initial_max_value;
			bool optimize_control_point_time;
			bool flat_extrapolation;

			typedef std::unique_ptr< OpenSim::PiecewiseLinearFunction > FunctionUP;
			std::vector< FunctionUP > m_Functions;
			std::vector< String > m_MuscleNames;
			size_t m_MuscleCount;
			double init_min;
			double init_max;

		private: // non-copyable and non-assignable
			FeedForwardController( const FeedForwardController& );
			FeedForwardController& operator=( const FeedForwardController& );
		};
	}
}
