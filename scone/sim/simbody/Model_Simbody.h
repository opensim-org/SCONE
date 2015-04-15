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

			virtual Vec3 GetComPos() override;
			virtual Vec3 GetComVel() override;
			virtual Real GetMass() override;
			virtual Vec3 GetGravity() override;
			virtual bool HasGroundContact() override;
			virtual Real GetTotalEnergyConsumption() override;

			virtual void AdvanceSimulationTo( double time ) override;
			virtual String WriteStateHistory( const String& file_base ) override;

			virtual void SetTerminationRequest() override;

			virtual std::map< String, double > GetState() override;
			virtual double GetTime() override;
			virtual double GetPreviousTime() override;
			virtual int GetIntegrationStep() override;
			virtual int GetPreviousIntegrationStep() override;

			/// Get the OpenSim model attached to this model
			OpenSim::Model& GetOsimModel() { return *m_pOsimModel; }
			const OpenSim::Model& GetOsimModel() const { return *m_pOsimModel; }
			SimTK::Integrator& GetTkIntegrator() { return *m_pTkIntegrator; }
			SimTK::State& GetTkState() { return *m_pTkState; }
			void SetTkState( SimTK::State& s ) { m_pTkState = &s; }

			virtual std::ostream& ToStream( std::ostream& str ) const override;

			// a signature describing the model
			virtual String GetSignature();

		private:
			void ReadState( const String& file );
			void CreateModelWrappers();
			LinkUP CreateLinkHierarchy( OpenSim::Body& osBody, Link* parent = nullptr );

			double integration_accuracy;
			double max_step_size;
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
