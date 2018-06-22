#pragma once

#include "scone/core/propnode_tools.h"
#include "scone/model/Controller.h"
#include "scone/core/HasName.h"

namespace scone
{
	class SCONE_API Measure : public Controller, public HasName
	{
	public:
		Measure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~Measure() { };

		virtual double GetResult( Model& model ) = 0;
		PropNode& GetReport() { return report; }
		const PropNode& GetReport() const { return report; }

		virtual const String& GetName() const override { return name; }
		Real GetWeight() { return weight; }
		Real GetThreshold() { return threshold; }
		Real GetOffset() { return offset; }
		bool GetMinimize() { return minimize; }

		virtual UpdateResult UpdateAnalysis( const Model& model, double timestamp ) override final;
		virtual UpdateResult UpdateControls( Model& model, double timestamp ) override final { return Controller::NoUpdate; }

	protected:
		virtual UpdateResult UpdateMeasure( const Model& model, double timestamp ) = 0;
		virtual bool IsActive( const Model& model, TimeInSeconds timestamp ) const { return timestamp >= start_time; }

		TimeInSeconds start_time;
		PropNode report;
		String name;
		Real weight;
		Real threshold;
		Real offset;
		bool minimize;
	};
}
