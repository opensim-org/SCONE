/*
** ReflexController.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/core/types.h"
#include "scone/core/PropNode.h"
#include "scone/optimization/Params.h"
#include "scone/model/Model.h"
#include "scone/model/Location.h"

namespace scone
{
	/// Controller that simulates reflexes with time delays.
	/// See Reflex and its subclasses for the various reflexes that can be added to this Controller.
	class ReflexController : public Controller
	{
	public:
		ReflexController( const PropNode& props, Params& par, Model& model, const Location& target_area );
		ReflexController( const ReflexController& other ) = delete;
		ReflexController operator=( const ReflexController& other ) = delete;
		virtual ~ReflexController();

		/// Indicate if reflexes parameters should be the same for left and right; default = 1.
		bool symmetric;

		/// Indicate if reflexes should be generated for both sides; default = 1.
		bool dual_sided;

		/// Child node containing all reflexes.
		const PropNode* Reflexes;

		virtual bool ComputeControls( Model& model, double timestamp ) override;
		virtual String GetClassSignature() const override;
		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	private:
		std::vector< ReflexUP > m_Reflexes;
	};
}
