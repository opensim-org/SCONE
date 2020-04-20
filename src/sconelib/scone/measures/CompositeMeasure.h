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

	The ''dual_sided'' parameter can be used to create equal measures for both sides of the body, for example:
	\verbatim
	CompositeMeasure {
		dual_sided = 1
		# this next measure is created for both ankle_angle_l and ankle_angle_r:
		DofMeasure {
			weight = 0.1
			dof = ankle_angle
			position { min = -60 max = 60 squared_penalty = 1 }
		}
		# this next measure is created for both knee_angle_l and knee_angle_r:
		DofMeasure {
			weight = 0.01
			threshold = 5
			dof = knee_angle
			force { min = 0 max = 0 abs_penalty = 1 }
		}
	}
	\endverbatim
	*/
	class CompositeMeasure : public Measure
	{
	public:
		CompositeMeasure( const PropNode& props, Params& par, const Model& model, const Location& loc );

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double ComputeResult( const Model& model ) override;

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
