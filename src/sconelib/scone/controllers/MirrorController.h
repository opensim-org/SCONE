/*
** MirrorController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/controllers/Controller.h"
#include "scone/core/Factories.h"

namespace scone
{
	/// Controller that automatically adds a mirrored version for each child Controller.
	class MirrorController : public Controller
	{
	public:
		MirrorController( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~MirrorController();

		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override;
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override;
		virtual bool ComputeControls( Model& model, double timestamp ) override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		ControllerUP c0, c1;
	};
}
