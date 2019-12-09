/*
** ModelOpenSim3.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/Model.h"

#include "BodyOpenSim3.h"
#include "MuscleOpenSim3.h"

#include <memory>

namespace OpenSim
{
	class Object;
	class Body;
	class Model;
	class Manager;
	class Probe;
	class PointActuator;
	class ConstantForce;
}

namespace SimTK
{
	class State;
	class Integrator;
	class TimeStepper;
}

namespace xo
{
	class pattern_matcher;
}

namespace scone
{
	class SimulationOpenSim3;
	class ControllerDispatcher;

	/// Model of type OpenSim3.
	class SCONE_OPENSIM_3_API ModelOpenSim3 : public Model
	{
	public:
		/// File containing the OpenSim model.
		path model_file;

		/// Integration method, options are: SemiExplicitEuler, SemiExplicitEuler2 (default), RungeKutta2, RungeKutta3, RungeKuttaMerson.
		String integration_method;

		/// Accuracy parameter for integration; default = 0.001.
		double integration_accuracy;

		/// Name of the DOF that needs to be adjusted to find the required initial_load; default = pelvis_ty.
		String initial_load_dof;

		/// Boolean that must be set before external forces can be added to the model; default = (automatic).
		bool enable_external_forces;

		/// Unsided name of the leg upper body (if any); default = femur
		String leg_upper_body;

		/// Unsided name of the leg lower body (if any); default = talus
		String leg_lower_body;

		/// Unsided name of the leg contact force (if any); default = foot
		String leg_contact_force;

		ModelOpenSim3( const PropNode& props, Params& par );
		virtual ~ModelOpenSim3();

		virtual path GetModelFile() const { return model_file; }

		virtual Vec3 GetComPos() const override;
		virtual Vec3 GetComVel() const override;
		virtual Vec3 GetComAcc() const override;
		virtual Real GetTotalEnergyConsumption() const override;

		virtual Real GetMass() const override final { return m_Mass; }
		virtual Real GetBW() const override final { return m_BW; }
		virtual Vec3 GetGravity() const override final;

		virtual void AdvanceSimulationTo( double time ) override;

		virtual double GetSimulationEndTime() const override;
		virtual void SetSimulationEndTime( double t ) override;
		virtual std::vector<xo::path> WriteResults( const path& file_base ) const override;

		virtual void RequestTermination();

		virtual double GetTime() const override;
		virtual double GetPreviousTime() const override;
		virtual int GetIntegrationStep() const override;
		virtual int GetPreviousIntegrationStep() const override;
		virtual TimeInSeconds GetSimulationStepSize() override;

		/// Get the OpenSim model attached to this model
		OpenSim::Model& GetOsimModel() { return *m_pOsimModel; }
		const OpenSim::Model& GetOsimModel() const { return *m_pOsimModel; }
		SimTK::Integrator& GetTkIntegrator() { return *m_pTkIntegrator; }
		const SimTK::Integrator& GetTkIntegrator() const { return *m_pTkIntegrator; }
		SimTK::State& GetTkState() { return *m_pTkState; }
		const SimTK::State& GetTkState() const { return *m_pTkState; }
		void SetTkState( SimTK::State& s ) { m_pTkState = &s; }

		virtual const String& GetName() const override;

		void ValidateDofAxes();
		void StoreCurrentFrame() override;
		void UpdateOsimStorage();

		OpenSim::ConstantForce* GetOsimBodyForce( index_t idx ) { return idx < m_BodyForces.size() ? m_BodyForces.at( idx ) : nullptr; }

		virtual const State& GetState() const override { return m_State; }
		virtual void SetState( const State& state, TimeInSeconds timestamp ) override;
		virtual void SetStateValues( const std::vector< Real >& state, TimeInSeconds timestamp ) override;

		virtual void SetController( ControllerUP c ) override;
		void InitializeOpenSimMuscleActivations( double override_activation = 0.0 );

	private:
		void InitStateFromTk();
		void CopyStateFromTk();
		void CopyStateToTk();
		void ReadState( const path& file );
		void FixTkState( double force_threshold = 0.1, double fix_accuracy = 0.1 );

		void CreateModelWrappers( const PropNode& pn, Params& par );
		OpenSim::Object& FindOpenSimObject( const String& name );
		void SetOpenSimObjectProperies( OpenSim::Object& os_obj, const PropNode& props, Params& par );
		void SetProperties( const PropNode& pn, Params& par );

		// #todo: remove this function and just create Joints instead
		LinkUP CreateLinkHierarchy( const OpenSim::Body& osBody, Link* parent = nullptr );

		std::unique_ptr< OpenSim::Model > m_pOsimModel;
		std::unique_ptr< OpenSim::Manager > m_pOsimManager;
		std::unique_ptr< SimTK::Integrator > m_pTkIntegrator;
		std::unique_ptr< SimTK::TimeStepper > m_pTkTimeStepper;
		SimTK::State* m_pTkState; // non-owning state reference
		OpenSim::Probe* m_pProbe; // owned by OpenSim::Model
		std::vector< OpenSim::ConstantForce* > m_BodyForces;

		friend ControllerDispatcher;
		ControllerDispatcher* m_pControllerDispatcher; // owned by OpenSim::Model

		State m_State; // model state
		int m_PrevIntStep;
		double m_PrevTime;

		// cached variables
		Real m_Mass;
		Real m_BW;
	};
}
