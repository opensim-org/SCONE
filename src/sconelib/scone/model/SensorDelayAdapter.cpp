/*
** SensorDelayAdapter.cpp
**
** Copyright (C) 2013-2021 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#include "Model.h"
#include "SensorDelayAdapter.h"
#include "Sensor.h"
#include "scone/core/platform.h"
#include "scone/core/Storage.h"
#include "scone/core/string_tools.h"
#include "xo/numerical/math.h"

namespace scone
{
	SensorDelayAdapter::SensorDelayAdapter( Model& model, Sensor& source, TimeInSeconds default_delay ) :
	Sensor(),
	m_Model( model ),
	m_InputSensor( source ),
	m_Delay( default_delay )
	{
		m_StorageIdx = m_Model.GetSensorDelayStorage().AddChannel( source.GetName() );
	}

	SensorDelayAdapter::~SensorDelayAdapter()
	{}

	Real SensorDelayAdapter::GetValue() const
	{
		return GetValue( m_Delay );
	}

	Real SensorDelayAdapter::GetValue( Real delay ) const
	{
		return m_Model.GetSensorDelayStorage().GetInterpolatedValue( m_Model.GetTime() - delay * m_Model.sensor_delay_scaling_factor, m_StorageIdx );
	}

	Real SensorDelayAdapter::GetAverageValue( int delay_samples, int window_size ) const
	{
		auto& sto = m_Model.GetSensorDelayStorage();
		auto history_begin = xo::max( 0, (int)sto.GetFrameCount() - delay_samples - window_size / 2 );
		auto history_end = xo::clamped( (int)sto.GetFrameCount() - delay_samples - window_size / 2 + window_size, 1, (int)sto.GetFrameCount() );

		Real value = 0.0;
		for ( auto i = history_begin; i < history_end; ++i )
			value += sto.GetFrame( i )[ m_StorageIdx ];
		return value / ( history_end - history_begin );
	}

	void SensorDelayAdapter::UpdateStorage()
	{
		Storage< Real >& storage = m_Model.GetSensorDelayStorage();
		SCONE_ASSERT( !storage.IsEmpty() && storage.Back().GetTime() == m_Model.GetTime() );
		storage.Back()[ m_StorageIdx ] = m_InputSensor.GetValue();
	}

	String SensorDelayAdapter::GetName() const
	{
		return m_InputSensor.GetName();
	}
}
