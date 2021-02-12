/*
** ModelOpenSim4.h
**
** Copyright (C) 2013-2019 Thomas Geijtenbeek and contributors. All rights reserved.
**
** This file is part of SCONE. For more information, see http://scone.software.
*/

#pragma once

#include "platform.h"
#include "scone/model/Model.h"

#include "BodyOpenSim4.h"
#include "MuscleOpenSim4.h"

#include <memory>
#include <map>

#include "ConstantForce.h"

namespace OpenSim
{
	class Object;
	class Body;
	class PhysicalFrame;
	class Model;
	class Manager;
	class Probe;
	class PointActuator;
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
	class SimulationOpenSim4;
	class ControllerDispatcher;

	/// Model of type OpenSim4.
	class SCONE_OPENSIM_4_API ModelOpenSim4 : public Model
	{
	public:
		ModelOpenSim4( const PropNode& props, Params& par );
		virtual ~ModelOpenSim4();

		/// File containing the OpenSim model.
		String model_file;

		/// Integration method, options are:
		/// RungeKutta2
		///	RungeKutta3
		/// RungeKuttaMerson
		/// SemiExplicitEuler2 (default)
		String integration_method;

		/// Accuracy parameter for integration; default = 0.001.
		double integration_accuracy;

		/// Name of the DOF that needs to be adjusted to find the required initial_load; default = /jointset/ground_pelvis/pelvis_ty/value.
		String initial_load_dof;

		/// Boolean that must be set before external forces can be added to the model; default = (automatic).
		bool create_body_forces;

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
		virtual std::ostream& ToStream( std::ostream& str ) const override;

		void ValidateDofAxes();
		void StoreCurrentFrame() override;
		void UpdateOsimStorage();

		OpenSim::ConstantForce* GetOsimBodyForce( index_t idx ) { return idx < m_BodyForces.size() ? m_BodyForces.at( idx ) : nullptr; }

		virtual const State& GetState() const override { return m_State; }
		virtual State& GetState() override { return m_State; }
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
		void SetModelProperties( const PropNode &pn, Params& par );
		void SetOpenSimProperties( const PropNode& pn, Params& par );
		void SetOpenSimProperty( OpenSim::Object& os, const PropNode& pn, Params& par );

		LinkUP CreateLinkHierarchy( const OpenSim::PhysicalFrame& osBody, Link* parent = nullptr );

		int m_PrevIntStep;
		double m_PrevTime;
		double m_FinalTime;

		std::unique_ptr< OpenSim::Model > m_pOsimModel;
		std::unique_ptr< OpenSim::Manager > m_pOsimManager;
		std::unique_ptr< SimTK::Integrator > m_pTkIntegrator;
		int m_integratorMethod = -1;
		std::unique_ptr< SimTK::TimeStepper > m_pTkTimeStepper;
		SimTK::State* m_pTkState; // non-owning state reference
		OpenSim::Probe* m_pProbe; // owned by OpenSim::Model

		std::vector< OpenSim::ConstantForce* > m_BodyForces;
		State m_State; // model state

		friend ControllerDispatcher;
		ControllerDispatcher* m_pControllerDispatcher; // owned by OpenSim::Model

		// cached variables
		Real m_Mass;
		Real m_BW;
	};
}
