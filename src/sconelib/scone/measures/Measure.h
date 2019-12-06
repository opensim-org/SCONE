/*
** Measure.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "scone/controllers/Controller.h"
#include "scone/core/HasName.h"

namespace scone
{
	/// Base class for Measures.
	class SCONE_API Measure : public Controller
	{
	public:
		Measure( const PropNode& props, Params& par, const Model& model, const Location& loc );
		virtual ~Measure() = default;

		/// Name of the Measure, to be used in reporting; defaults to measure type
		mutable String name;

		/// Weighting factor applied to the result of the measure; default = 1.
		Real weight;

		/// Threshold above / below which the measure becomes zero; default = 0.
		Real threshold;

		/// Offset added to measure result; default = 0.
		Real result_offset;

		/// Indicate whether this measure should be minimized; default value depends on the measure type (usually true).
		bool minimize;

		double GetResult( const Model& model );
		double GetWeightedResult( const Model& model );

		PropNode& GetReport() { return report; }
		const PropNode& GetReport() const { return report; }
	
		virtual const String& GetName() const override;
		Real GetWeight() { return weight; }
		Real GetThreshold() { return threshold; }
		Real GetOffset() { return result_offset; }
		bool GetMinimize() { return minimize; }

	protected:
		virtual double ComputeResult( const Model& model ) = 0;
		virtual bool ComputeControls( Model& model, double timestamp ) override final { return false; }
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override final;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) = 0;
		double WorstResult() const;

		PropNode report;
		xo::optional< double > result; // caches result so it's only computed once
	};
}
