#pragma once

#include "sim.h"
#include "Sensor.h"

namespace scone
{
	namespace sim
	{
		// Base class for muscle sensors
		class SCONE_SIM_API MuscleSensor : public Sensor
		{
		public:
			MuscleSensor( Muscle& m ) : m_Muscle( m ) {}
			MuscleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual const String& GetSourceName() const override;
		protected:
			Muscle& m_Muscle;
		};

		// Sensor for normalized muscle force
		class SCONE_SIM_API MuscleForceSensor : public MuscleSensor
		{
		public:
			MuscleForceSensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleForceSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor for normalized muscle length
		class SCONE_SIM_API MuscleLengthSensor : public MuscleSensor
		{
		public:
			MuscleLengthSensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleLengthSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor for normalized muscle lengthening speed
		class SCONE_SIM_API MuscleVelocitySensor : public MuscleSensor
		{
		public:
			MuscleVelocitySensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleVelocitySensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor that simulates (based on [Prochazka 1999])
		class SCONE_SIM_API MuscleSpindleSensor : public MuscleSensor
		{
		public:
			MuscleSpindleSensor( Muscle& m ) : MuscleSensor( m ) {}
			MuscleSpindleSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : MuscleSensor( pn, par, model, target_area ) { };
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Base class for dof sensors
		class SCONE_SIM_API DofSensor : public Sensor
		{
		public:
			DofSensor( Dof& dof ) : m_Dof( dof ), m_pRootDof( nullptr ) {}
			DofSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual const String& GetSourceName() const override;
		protected:
			Dof& m_Dof;
			Dof* m_pRootDof;
		};

		class SCONE_SIM_API DofPositionSensor : public DofSensor
		{
		public:
			DofPositionSensor( Dof& dof ) : DofSensor( dof ) {}
			DofPositionSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : DofSensor( pn, par, model, target_area ) { m_pRootDof = nullptr; }
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		class SCONE_SIM_API DofVelocitySensor : public DofSensor
		{
		public:
			DofVelocitySensor( Dof& dof ) : DofSensor( dof ) {}
			DofVelocitySensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area ) : DofSensor( pn, par, model, target_area ) { m_pRootDof = nullptr; }
			virtual Real GetValue() const override;
			virtual String GetName() const override;
		};

		// Sensor for normalized leg load, based on target_area
		class SCONE_SIM_API LegLoadSensor : public Sensor
		{
		public:
			LegLoadSensor( Leg& leg ) : m_Leg( leg ) {}
			LegLoadSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			virtual Real GetValue() const override;
			virtual String GetName() const override;
			virtual const String& GetSourceName() const override;
		protected:
			Leg& m_Leg;
		};

		// Sensor to measure the sagittal orientation of a body in world coordinates
		class SCONE_SIM_API OrientationSensor : public Sensor
		{
		public:
			enum Plane { Invalid = -1, Sagittal, Coronal, Transverse };
			OrientationSensor( const PropNode& pn, opt::ParamSet& par, sim::Model& model, const Area& target_area );
			OrientationSensor( Model& model, Plane plane, Real posgain = 1.0, Real velgain = 0.0 ); 
			virtual Real GetValue() const override;
			virtual String GetName() const override;
			virtual const String& GetSourceName() const override;

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
