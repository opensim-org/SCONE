#pragma once

#include "cs.h"
#include "../sim/Controller.h"
#include "../core/PropNode.h"
#include "../opt/ParamSet.h"
#include "../core/Factory.h"
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

			Function* CreateFunction( opt::ParamSet &par, const String& prefix );

			virtual void UpdateControls( sim::Model& model, double timestamp ) override;
			virtual void Initialize( sim::Model& model ) override;

			bool UseModes() { return number_of_modes > 0; }

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

			// muscle info
			struct ActInfo
			{
				ActInfo() : side( Middle ), function_idx( NO_INDEX ) {};
				String name;
				Side side;
				String full_name;
				size_t function_idx;
				std::vector< double > mode_weights;
			};

			typedef std::unique_ptr< Function > FunctionUP;
			std::vector< FunctionUP > m_Functions;
			std::vector< ActInfo > m_ActInfos;

		private: // non-copyable and non-assignable
			FeedForwardController( const FeedForwardController& );
			FeedForwardController& operator=( const FeedForwardController& );
		};
	}
}
