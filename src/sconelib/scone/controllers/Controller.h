/*
** Controller.h
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/core/HasSignature.h"
#include "scone/core/HasData.h"
#include "scone/model/State.h"
#include "scone/optimization/Params.h"
#include "xo/filesystem/path.h"
#include "scone/core/HasName.h"

namespace scone
{
	/// Base class for SCONE Controllers. See derived classes for specific functionality.
	class SCONE_API Controller : public HasSignature, public HasData, public HasName
	{
	public:
		Controller( const PropNode& props, Params& par, Model& model, const Location& target_area );
		virtual ~Controller();

		/// Time [s] at which Controller becomes active; default = 0.
		TimeInSeconds start_time;

		/// Time [s] at which Controller becomes inactive; default = until simulation ends.
		TimeInSeconds stop_time;

		/// Name of the controller, uses as a prefix for the control parameters; empty by default
		String name;

		// Called each step, returns true on termination request, checks IsActive() first
		bool UpdateControls( Model& model, double timestamp );

		// Called after each successful integration step, returns true on termination request, checks IsActive() first
		bool UpdateAnalysis( const Model& model, double timestamp );

		// Check if Controller is active, i.e. start_time >= time_stamp > stop_time && disabled state is not set
		virtual bool IsActive( const Model& model, double time ) { return time >= start_time && ( stop_time == 0.0 || time < stop_time ) && !disabled_; }

		// Set the disabled state of the controller, this is checked in IsActive().
		void SetDisabled( bool disabled ) { disabled_ = disabled; }

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override {}
		virtual std::vector<xo::path> WriteResults( const xo::path& file ) const { return std::vector<xo::path>(); }

		virtual const String& GetName() const override { return name; }

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) { return false; }
		virtual bool PerformAnalysis( const Model& model, double timestamp ) { return false; }

		bool disabled_;
	};
}
