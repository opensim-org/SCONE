/*
** SimpleMuscleReflex.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Reflex.h"
#include "scone/model/SensorDelayAdapter.h"

namespace scone
{
	class SimpleMuscleReflex : public Reflex
	{
	public:
		SimpleMuscleReflex( const String& target, const String& source, double mean, double stdev, TimeInSeconds delay, Model& model, Params& par, const Location& loc );
		virtual ~SimpleMuscleReflex() {}

		virtual void ComputeControls( double timestamp ) override;
		virtual void StoreData( Storage<Real>::Frame& frame, const StoreDataFlags& flags ) const override {}

	private:
		SensorDelayAdapter* m_Source;
		double m_Gain;
	};
}
