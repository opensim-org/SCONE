#pragma once

#include "scone/controllers/Controller.h"
#include "scone/core/HasName.h"

namespace scone
{
	// Class: Measure
	class SCONE_API Measure : public Controller, public HasName
	{
	public:
		Measure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~Measure() { };

		// Prop: name
		// Name of the Measure
		String name;

		// Prop: weight
		// Weight at which to apply the measure
		Real weight;

		/// Threshold below which the measure becomes zero
		Real threshold;

		/// Offset added to measure result
		Real offset;

		/// Indicate whether this measure should be minimized
		bool minimize;

		virtual double GetResult( Model& model ) = 0;
		PropNode& GetReport() { return report; }
		const PropNode& GetReport() const { return report; }

		virtual const String& GetName() const override { return name; }
		Real GetWeight() { return weight; }
		Real GetThreshold() { return threshold; }
		Real GetOffset() { return offset; }
		bool GetMinimize() { return minimize; }

	protected:
		virtual bool ComputeControls( Model& model, double timestamp ) override final { return false; }
		virtual bool PerformAnalysis( const Model& model, double timestamp ) override final;
		virtual bool UpdateMeasure( const Model& model, double timestamp ) = 0;

		PropNode report;
	};
}
