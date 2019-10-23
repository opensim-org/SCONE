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
	/** Individual child measures are evaluated and summed up, respecting the ''weight'',
	''threshold'' and ''offset'' parameters of the indiviudal measures (see Measure). Example:
	\verbatim
	CompositeMeasure {
		GaitMeasure {
			weight = 100
			threshold = 0.5
			...
		}
		EffortMeasure {
			weight = 0.1
			...
		}
	}
	\endverbatim
	*/
	class CompositeMeasure : public Measure
	{
	public:
		CompositeMeasure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~CompositeMeasure();
		CompositeMeasure( CompositeMeasure& other ) = delete;
		CompositeMeasure& operator=( CompositeMeasure& other ) = delete;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( Model& model ) override;

		const PropNode* Measures;

		/// Create symmetric measures for both sides; default = false.
		bool dual_sided;

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< MeasureUP > m_Measures;
	};
}
