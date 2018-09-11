#pragma once

#include "scone/core/types.h"
#include "scone/core/HasSignature.h"
#include "scone/core/HasData.h"
#include "scone/model/State.h"
#include "scone/optimization/Params.h"
#include "xo/filesystem/path.h"

namespace scone
{
	// Class: Controller
	class Controller : public HasSignature, public HasData
	{
	public:
		Controller( const PropNode& props, Params& par, Model& model, const Locality& target_area );
		virtual ~Controller();

		// Method: UpdateControls
		// Called each step, returns true on termination request, checks IsActive() first
		bool UpdateControls( Model& model, double timestamp );

		/// Called after each successful integration step, returns true on termination request
		bool UpdateAnalysis( const Model& model, double timestamp );

		/// Check if Controller is active, i.e. start_time >= time_stamp > stop_time && disabled state is not set
		virtual bool IsActive( const Model& model, double time ) { return time >= start_time && time < stop_time && !disabled_; }

		/// Sets or clears the Controller's 'disabled' state
		void SetDisabled( bool d ) { disabled_ = d; }

		/// default implementation doesn't store anything
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}

		/// default implementation doesn't store anything
		virtual std::vector<xo::path> WriteResults( const xo::path& file ) const { return std::vector<xo::path>(); }

		// Prop: start_time
		// Time at which Controller becomes active
		double start_time;

		// Prop: stop_time
		// Time at which Controller becomes inactive
		double stop_time;

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) { return false; }
		virtual bool PerformAnalysis( const Model& model, double timestamp ) { return false; }

		bool disabled_;
	};
}
