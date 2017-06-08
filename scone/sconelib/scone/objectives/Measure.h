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
		virtual bool IsActive( const Model& model, TimeInSeconds timestamp ) const { return timestamp >= start_time; }

		virtual const String& GetName() const override { return name; }
		Real GetWeight() { return weight; }
		Real GetThreshold() { return threshold; }
		Real GetOffset() { return offset; }

	private:
		TimeInSeconds start_time;
		PropNode report;

		String name;
		Real weight;
		Real threshold;
		Real offset;
	};
}
