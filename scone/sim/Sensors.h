#pragma once

#include "sim.h"
#include "Sensor.h"

namespace scone
{
	namespace sim
	{
		// Base class for muscle sensors
		class SCONE_API MuscleSensor : public Sensor
		{
		public:
			MuscleSensor( Muscle& m ) : m_Muscle( m ) {}
			MuscleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
		protected:
			Muscle& m_Muscle;
		};

		// Sensor for normalized muscle force
		class SCONE_API MuscleForceSensor : public MuscleSensor
		{
		public:
			MuscleForceSensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleForceSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor for normalized muscle length
		class SCONE_API MuscleLengthSensor : public MuscleSensor
		{
		public:
			MuscleLengthSensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleLengthSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor for normalized muscle lengthening speed
		class SCONE_API MuscleVelocitySensor : public MuscleSensor
		{
		public:
			MuscleVelocitySensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleVelocitySensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor that simulates (based on [Prochazka 1999])
		class SCONE_API MuscleSpindleSensor : public MuscleSensor
		{
		public:
			MuscleSpindleSensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleSpindleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Base class for dof sensors
		class SCONE_API DofSensor : public Sensor
		{
		public:
			DofSensor( Dof& dof ) : m_Dof( dof ), m_pRootDof( nullptr ) {}
			DofSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
		protected:
			Dof& m_Dof;
			Dof* m_pRootDof;
		};

		class SCONE_API DofPositionSensor : public DofSensor
		{
		public:
			DofPositionSensor( Dof& dof ) : DofSensor( dof ) {}
			DofPositionSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : DofSensor( pn, par, model, target_area ) { m_pRootDof = nullptr; }
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		class SCONE_API DofVelocitySensor : public DofSensor
		{
		public:
			DofVelocitySensor( Dof& dof ) : DofSensor( dof ) {}
			DofVelocitySensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : DofSensor( pn, par, model, target_area ) { m_pRootDof = nullptr; }
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor for normalized leg load, based on target_area
		class SCONE_API LegLoadSensor : public Sensor
		{
		public:
			LegLoadSensor( Leg& leg ) : m_Leg( leg ) {}
			LegLoadSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		protected:
			Leg& m_Leg;
		};

		// Base class for body sensors
		class SCONE_API BodySensor : public Sensor
		{
		public:
			BodySensor( Body& body ) : m_Body( body ) {}
			BodySensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
		protected:
			Body& m_Body;
		};

		class SCONE_API BodyOriSensor : public BodySensor
		{
		public:
			BodyOriSensor( Body& body ) : BodySensor( body ) {}
			virtual String GetName() const override;
			virtual Count GetChannelCount() override { return 3; }
			virtual Real GetValue( Index idx ) const override;
		};

		class SCONE_API BodyAngVelSensor : public BodySensor
		{
		public:
			BodyAngVelSensor( Body& body ) : BodySensor( body ) {}
			virtual String GetName() const override;
			virtual Count GetChannelCount() override { return 3; }
			virtual Real GetValue( Index idx ) const override;
		};

		// Sensor to measure the sagittal orientation of a body in world coordinates
		class SCONE_API OrientationSensor : public Sensor
		{
		public:
			enum Plane { Invalid = -1, Sagittal, Coronal, Transverse };
			OrientationSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
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
}
