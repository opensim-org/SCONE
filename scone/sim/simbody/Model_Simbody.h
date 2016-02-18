#pragma once

#include "sim_simbody.h"
#include "../Model.h"

#include "Body_Simbody.h"
#include "Muscle_Simbody.h"

#include <memory>
#include <map>

namespace OpenSim
{
	class Model;
	class Manager;
	class Probe;
}

namespace SimTK
{
	class State;
	class Integrator;
}

namespace scone
{
	namespace sim
	{
		class Simulation_Simbody;

		class SCONE_SIM_SIMBODY_API Model_Simbody : public Model
		{
		public:
			Model_Simbody( const PropNode& props, opt::ParamSet& par );
			virtual ~Model_Simbody();

			virtual Vec3 GetComPos() const override;
			virtual Vec3 GetComVel() const override;
			virtual Real GetMass() const override;
			virtual Vec3 GetGravity() const override;
			virtual Real GetTotalEnergyConsumption() const override;

			virtual bool AdvanceSimulationTo( double time ) override;

			virtual double GetSimulationEndTime() const override;
			virtual String WriteData( const String& file_base ) const override;

			virtual void SetTerminationRequest() override;

			virtual double GetTime() const override;
			virtual double GetPreviousTime() const override;
			virtual int GetIntegrationStep() const override;
			virtual int GetPreviousIntegrationStep() const override;

			/// State access
			virtual std::vector< Real > GetStateValues() const override;
			virtual void SetStateValues( const std::vector< Real >& values ) override;
			virtual std::vector< String > GetStateVariableNames() const override;
			Real GetStateVariable( const String& name ) const override;
			virtual void SetStateVariable( const String& name, Real value ) override;

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

		protected:
			virtual String GetClassSignature() const override;

		private:
			void SetOpenSimParameters( const PropNode& name, opt::ParamSet& par );
			void FixState( double force_threshold = 0.1, double fix_accuracy = 0.1 );
			State ReadState( const String& file );
			void CreateModelWrappers();
			LinkUP CreateLinkHierarchy( OpenSim::Body& osBody, Link* parent = nullptr );
			void SetStateVariables( const std::map< String, Real >& state );

			virtual void SetStoreData( bool store ) override;

			String integration_method;
			double integration_accuracy;
			double max_step_size;
			bool use_fixed_control_step_size;
			double fixed_control_step_size;
			String model_file;

			int m_PrevIntStep;
			double m_PrevTime;

			std::unique_ptr< OpenSim::Model > m_pOsimModel;
			std::unique_ptr< OpenSim::Manager > m_pOsimManager;
			std::unique_ptr< SimTK::Integrator > m_pTkIntegrator;
			SimTK::State* m_pTkState; // non-owning state reference
			OpenSim::Probe* m_pProbe; // owned by OpenSim::Model

			class ControllerDispatcher;
			friend ControllerDispatcher;
			ControllerDispatcher* m_pControllerDispatcher; // owned by OpenSim::Model
		};
	}
}
