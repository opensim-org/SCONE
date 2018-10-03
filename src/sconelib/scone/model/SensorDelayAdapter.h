/*
** SensorDelayAdapter.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Sensor.h"

namespace scone
{
	struct SCONE_API SensorDelayAdapter : public Sensor
	{
		SensorDelayAdapter( Model& model, Sensor& source, TimeInSeconds default_delay );
		virtual ~SensorDelayAdapter();

		virtual Real GetValue() const override;
		virtual String GetName() const override;

		Real GetValue( Real delay ) const;
		Real GetAverageValue( int delay_samples, int window_size ) const;

		void UpdateStorage();
		Sensor& GetInputSensor() { return m_InputSensor; }

	private:
		Model& m_Model;
		Sensor& m_InputSensor;
		TimeInSeconds m_Delay;
		index_t m_StorageIdx;
	};
}
