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
	class Measure : public Controller, public HasName
	{
	public:
		Measure( const PropNode& props, Params& par, Model& model, const Location& loc );
		virtual ~Measure() { };

		/// Name of the Measure, to be used in reporting.
		String name;

		/// Weighting factor applied to the result of the measure; default = 1.
		Real weight;

		/// Threshold above / below which the measure becomes zero; default = 0.
		Real threshold;

		/// Offset added to measure result; default = 0.
		Real result_offset;

		/// Indicate whether this measure should be minimized; is set correctly by default.
		bool minimize;

		double GetResult( Model& model );
		double GetWeightedResult( Model& model );

		PropNode& GetReport() { return report; }
		const PropNode& GetReport() const { return report; }
	
		virtual const String& GetName() const override { return name; }
		Real GetWeight() { return weight; }
		Real GetThreshold() { return threshold; }
		Real GetOffset() { return result_offset; }
		bool GetMinimize() { return minimize; }

	protected:
		virtual double ComputeResult( Model& model ) = 0;
		virtual bool ComputeControls( Model& model, double timestamp ) override final { return false; }
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override final;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) = 0;

		PropNode report;
		xo::optional< double > result;
	};
}
