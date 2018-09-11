#pragma once

#include "Measure.h"
#include "scone/core/types.h"

namespace scone
{
	// Class: CompositeMeasure
	// Measure made up of different weighted Measures
	class CompositeMeasure : public Measure
	{
	public:
		CompositeMeasure( const PropNode& props, Params& par, Model& model, const Locality& area );
		virtual ~CompositeMeasure();
		CompositeMeasure( CompositeMeasure& other ) = delete;
		CompositeMeasure& operator=( CompositeMeasure& other ) = delete;

		virtual bool UpdateMeasure( const Model& model, double timestamp ) override;
		virtual double GetResult( Model& model ) override;

		// Prop: Measures
		// Group in which all child Measures can be stored

		virtual void StoreData( Storage< Real >::Frame& frame, const StoreDataFlags& flags ) const override;

	protected:
		virtual String GetClassSignature() const override;

	private:
		std::vector< MeasureUP > m_Measures;
	};
}
