/*
** CompositeController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/controllers/Controller.h"

namespace scone
{
	/// Controller consisting of multiple child controllers.
	/// Child Controllers are inserted as children of this parameter, e.g. ''CompositeController { FeedForwardController { ... } ReflexController { ... } }''.
	class CompositeController : public Controller
	{
	public:
		CompositeController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~CompositeController() {}
		
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual std::vector<xo::path> WriteResults( const xo::path& file ) const override;

		const PropNode* Controllers;

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override;

		virtual String GetClassSignature() const override;
		std::vector< ControllerUP > controllers_;
	};
}
