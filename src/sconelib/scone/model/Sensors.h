/*
** Sensors.h
**
** Copyright (C) 2013-2018 Thomas Geijtenbeek. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "Sensor.h"
#include "scone/core/types.h"
#include "scone/core/Vec3.h"

namespace scone
{
	// Base class for muscle sensors
	class SCONE_API MuscleSensor : public Sensor
	{
	public:
		MuscleSensor( Muscle& m ) : m_Muscle( m ) {}
	protected:
		Muscle& m_Muscle;
	};

	// Sensor for normalized muscle force
	class SCONE_API MuscleForceSensor : public MuscleSensor
	{
	public:
		MuscleForceSensor( Muscle& m ) : MuscleSensor( m ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor for normalized muscle length
	class SCONE_API MuscleLengthSensor : public MuscleSensor
	{
	public:
		MuscleLengthSensor( Muscle& m ) : MuscleSensor( m ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor for normalized muscle lengthening speed
	class SCONE_API MuscleVelocitySensor : public MuscleSensor
	{
	public:
		MuscleVelocitySensor( Muscle& m ) : MuscleSensor( m ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor that simulates Ia muscle spindle (based on [Prochazka 1999], p.135)
	class SCONE_API MuscleSpindleSensor : public MuscleSensor
	{
	public:
		MuscleSpindleSensor( Muscle& m ) : MuscleSensor( m ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor that simulates Ia muscle spindle (based on [Prochazka 1999], p.135)
	class SCONE_API MuscleExcitationSensor : public MuscleSensor
	{
	public:
		MuscleExcitationSensor( Muscle& m ) : MuscleSensor( m ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Base class for dof sensors
	class SCONE_API DofSensor : public Sensor
	{
	public:
		DofSensor( Dof& dof, Dof* root_dof ) : m_Dof( dof ), m_pRootDof( root_dof ) {}
	protected:
		Dof& m_Dof;
		Dof* m_pRootDof;
	};

	class SCONE_API DofPositionSensor : public DofSensor
	{
	public:
		DofPositionSensor( Dof& dof, Dof* root_dof = nullptr ) : DofSensor( dof, root_dof ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	class SCONE_API DofVelocitySensor : public DofSensor
	{
	public:
		DofVelocitySensor( Dof& dof, Dof* root_dof = nullptr ) : DofSensor( dof, root_dof ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	class SCONE_API DofPosVelSensor : public DofSensor
	{
	public:
		DofPosVelSensor( Dof& dof, double kv, Dof* root_dof = nullptr ) : DofSensor( dof, root_dof ), m_KV( kv ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
		double m_KV;
	};

	// Sensor for normalized leg load, based on target_area
	class SCONE_API LegLoadSensor : public Sensor
	{
	public:
		LegLoadSensor( const Leg& leg ) : m_Leg( leg ) {}
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	protected:
		const Leg& m_Leg;
	};

	// Base class for body sensors
	class SCONE_API BodySensor : public Sensor
	{
	public:
		BodySensor( Body& body ) : m_Body( body ) {}
	protected:
		Body& m_Body;
	};

	class SCONE_API BodyPositionSensor : public BodySensor
	{
	public:
		BodyPositionSensor( Body& body ) : BodySensor( body ) {}
	protected:
		Vec3 axes_weights;
	};

	class SCONE_API BodyOriSensor : public BodySensor
	{
	public:
		BodyOriSensor( Body& body ) : BodySensor( body ) {}
		virtual String GetName() const override;
		virtual size_t GetChannelCount() override { return 3; }
		virtual Real GetValue( index_t idx ) const override;
	};

	class SCONE_API BodyAngVelSensor : public BodySensor
	{
	public:
		BodyAngVelSensor( Body& body ) : BodySensor( body ) {}
		virtual String GetName() const override;
		virtual size_t GetChannelCount() override { return 3; }
		virtual Real GetValue( index_t idx ) const override;
	};
}
