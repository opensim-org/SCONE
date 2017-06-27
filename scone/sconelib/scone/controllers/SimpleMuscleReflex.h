#pragma once

#include "Reflex.h"
#include "../model/SensorDelayAdapter.h"

namespace scone
{
	class SimpleMuscleReflex : public Reflex
	{
	public:
		SimpleMuscleReflex( const String& target, const String& source, double mean, double stdev, TimeInSeconds delay, Model& model, Params& par, const Locality& area );
		virtual ~SimpleMuscleReflex() {}

		virtual void ComputeControls( double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) override;

	private:
		SensorDelayAdapter* m_Source;
		double m_Gain;
	};
}
