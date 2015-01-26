#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "Function.h"

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
			// function parameters
			String function_type;
			bool flat_extrapolation;
			bool use_symmetric_actuators;

			// control point settings
			size_t control_points;
			double control_point_time_delta;
			bool optimize_control_point_time;
			double init_min;
			double init_max;

			// mode settings
			size_t number_of_modes;
			double init_mode_weight_min;
			double init_mode_weight_max;

			typedef std::unique_ptr< Function > FunctionUP;
			std::vector< FunctionUP > m_Functions;
			std::vector< String > FunctionNames;
			std::vector< String > m_MuscleNames;
			std::vector< size_t > m_MuscleIndices;
			std::vector< std::vector< double > > m_MuscleModeWeights;

		private: // non-copyable and non-assignable
			FeedForwardController( const FeedForwardController& );
			FeedForwardController& operator=( const FeedForwardController& );
		};
	}
}
