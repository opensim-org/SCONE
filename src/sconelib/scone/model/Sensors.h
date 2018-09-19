#pragma once

#include "Sensor.h"
#include "scone/core/types.h"

namespace scone
{
	// Base class for muscle sensors
	class SCONE_API MuscleSensor : public Sensor
	{
	public:
		MuscleSensor( Muscle& m ) : m_Muscle( m ) {}
		MuscleSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area );
	protected:
		Muscle& m_Muscle;
	};

	// Sensor for normalized muscle force
	class SCONE_API MuscleForceSensor : public MuscleSensor
	{
	public:
		MuscleForceSensor( Muscle& m ) : MuscleSensor( m ) {}
		MuscleForceSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor for normalized muscle length
	class SCONE_API MuscleLengthSensor : public MuscleSensor
	{
	public:
		MuscleLengthSensor( Muscle& m ) : MuscleSensor( m ) {}
		MuscleLengthSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor for normalized muscle lengthening speed
	class SCONE_API MuscleVelocitySensor : public MuscleSensor
	{
	public:
		MuscleVelocitySensor( Muscle& m ) : MuscleSensor( m ) {}
		MuscleVelocitySensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor that simulates Ia muscle spindle (based on [Prochazka 1999], p.135)
	class SCONE_API MuscleSpindleSensor : public MuscleSensor
	{
	public:
		MuscleSpindleSensor( Muscle& m ) : MuscleSensor( m ) {}
		MuscleSpindleSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Sensor that simulates Ia muscle spindle (based on [Prochazka 1999], p.135)
	class SCONE_API MuscleExcitationSensor : public MuscleSensor
	{
	public:
		MuscleExcitationSensor( Muscle& m ) : MuscleSensor( m ) {}
		MuscleExcitationSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	// Base class for dof sensors
	class SCONE_API DofSensor : public Sensor
	{
	public:
		DofSensor( Dof& dof, Dof* root_dof ) : m_Dof( dof ), m_pRootDof( root_dof ) {}
		DofSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area );
	protected:
		Dof& m_Dof;
		Dof* m_pRootDof;
	};

	class SCONE_API DofPositionSensor : public DofSensor
	{
	public:
		DofPositionSensor( Dof& dof, Dof* root_dof = nullptr ) : DofSensor( dof, root_dof ) {}
		DofPositionSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : DofSensor( pn, par, model, target_area ) { m_pRootDof = nullptr; }
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	class SCONE_API DofVelocitySensor : public DofSensor
	{
	public:
		DofVelocitySensor( Dof& dof, Dof* root_dof = nullptr ) : DofSensor( dof, root_dof ) {}
		DofVelocitySensor( const PropNode& pn, Params& par, Model& model, const Location& target_area ) : DofSensor( pn, par, model, target_area ) { m_pRootDof = nullptr; }
		virtual Real GetValue() const override;
		virtual String GetName() const override;
	};

	class SCONE_API DofPosVelSensor : public DofSensor
	{
	public:
		DofPosVelSensor( Dof& dof, double kv, Dof* root_dof = nullptr ) : DofSensor( dof, root_dof ), m_KV( kv ) {}
		DofPosVelSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area );
		virtual Real GetValue() const override;
		virtual String GetName() const override;
		double m_KV;
	};

	// Sensor for normalized leg load, based on target_area
	class SCONE_API LegLoadSensor : public Sensor
	{
	public:
		LegLoadSensor( const Leg& leg ) : m_Leg( leg ) {}
		LegLoadSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area );
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
		BodySensor( const PropNode& pn, Params& par, Model& model, const Location& target_area );
	protected:
		Body& m_Body;
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

	// Sensor to measure the sagittal orientation of a body in world coordinates
	class SCONE_API OrientationSensor : public Sensor
	{
	public:
		enum Plane { Invalid = -1, Sagittal, Coronal, Transverse };
		OrientationSensor( const PropNode& pn, Params& par, Model& model, const Location& target_area );
		OrientationSensor( Model& model, Plane plane, Real posgain = 1.0, Real velgain = 0.0 );
		virtual Real GetValue() const override;
		virtual String GetName() const override;

	protected:
		// TODO: don't use Dofs, use Body instead
		Plane m_Plane;
		const Dof* m_Pelvis;
		const Dof* m_Lumbar;
		Real m_PosGain;
		Real m_VelGain;
	};
}
