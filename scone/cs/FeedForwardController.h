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
			FeedForwardController() : m_MuscleCount( 0 ) { };
			virtual ~FeedForwardController() { };

			virtual void ProcessProperties( const PropNode& props ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;
			virtual bool UpdateControls( sim::Model& model, double timestamp ) override;
			virtual void ConnectModel( sim::Model& model ) override;

		private:
			String function_type;
			bool use_symmetric_actuators;
			size_t control_points;
			double initial_time_separation;
			double initial_max_value;
			bool use_fixed_time;

			typedef std::unique_ptr< OpenSim::PiecewiseLinearFunction > FunctionUP;
			std::vector< FunctionUP > m_Functions;
			std::vector< String > m_MuscleNames;
			size_t m_MuscleCount;
			double init_mean;
			double init_std;
		private: // non-copyable and non-assignable
			FeedForwardController( const FeedForwardController& );
			FeedForwardController& operator=( const FeedForwardController& );
		};
	}
}
