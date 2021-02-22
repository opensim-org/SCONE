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

		/// File containing the initial state (or pose) of the model.
		path state_init_file;

		/// Integration method, options are: SemiExplicitEuler, SemiExplicitEuler2 (default), RungeKutta2, RungeKutta3, RungeKuttaMerson.
		String integration_method;

		/// Accuracy parameter for integration; default = 0.001.
		double integration_accuracy;

		/// Boolean that must be set before external forces can be added to the model; default = (automatic).
		bool enable_external_forces;

		/// Unsided name of the leg upper body (if any); default = femur
		String leg_upper_body;

		/// Unsided name of the leg lower body (if any), leave empty to use two bodies below leg_upper_body; default = ""
		String leg_lower_body;

		/// Unsided name of the leg contact force (if any); default = foot
		String leg_contact_force;

		/// ADVANCED: use extra thread safety, required due to issue with Millard2012EquilibriumMuscle; default = 1
		bool safe_mode;

		ModelOpenSim3( const PropNode& props, Params& par );
		virtual ~ModelOpenSim3();

		virtual path GetModelFile() const override { return model_file; }

		virtual Vec3 GetComPos() const override;
		virtual Vec3 GetComVel() const override;
		virtual Vec3 GetComAcc() const override;
		virtual Vec3 GetLinMom() const override;
		virtual Vec3 GetAngMom() const override;
		virtual std::pair<Vec3, Vec3> GetLinAngMom() const override;
		virtual Real GetTotalEnergyConsumption() const override;

		virtual Real GetMass() const override final { return m_Mass; }
		virtual Real GetBW() const override final { return m_BW; }
		virtual Vec3 GetGravity() const override final;

		virtual void AdvanceSimulationTo( double time ) override;

		virtual double GetSimulationEndTime() const override;
		virtual void SetSimulationEndTime( double t ) override;

		virtual void RequestTermination() override;

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

		// internal data
		// #todo: leave storage to children and create wrappers on request instead
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
