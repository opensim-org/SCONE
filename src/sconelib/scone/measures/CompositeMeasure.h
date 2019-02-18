/*
** CompositeMeasure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Measure.h"
#include "scone/core/types.h"

namespace scone
{
	/// Measure made up of different weighted Measures.
	/// Individual child measures should be added to the Measures group. Each measure is evaluated
	/// and added together respecting their individual weight, threshold and offset parameters (see Measure).
	class CompositeMeasure : public Measure
	{
	public:
		CompositeMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~CompositeMeasure();
		CompositeMeasure( CompositeMeasure& other ) = delete;
		CompositeMeasure& operator=( CompositeMeasure& other ) = delete;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( Model& model ) override;

		/// Child node containing all Measures.
		const PropNode* Measures;

		/// Create symmetric measures for both sides; default = false.
		bool symmetric;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< MeasureUP > m_Measures;
	};
}
