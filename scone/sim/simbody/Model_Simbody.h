#pragma once

#include "sim_simbody.h"
#include "../Model.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include <memory>

namespace OpenSim
{
	class Model;
	class Manager;
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

			virtual void AdvanceSimulationTo( double time ) override;
			virtual void WriteStateHistory( const String& file ) override;
			virtual void SetTerminationRequest() override;

			virtual double GetTime() override;
			virtual size_t GetStep() override;

			// process parameters for this model and attached controllers
			virtual void ProcessParameters( opt::ParamSet& par ) override;

			/// Get the OpenSim model attached to this model
			OpenSim::Model& GetOsimModel() { return *m_pOsimModel; }
			const OpenSim::Model& GetOsimModel() const { return *m_pOsimModel; }
			SimTK::Integrator& GetTkIntegrator() { return *m_pTkIntegrator; }
			SimTK::State& GetTkState() { return *m_pTkState; }
			void SetTkState( SimTK::State& s ) { m_pTkState = &s; }

			virtual std::ostream& ToStream( std::ostream& str ) const override;

		private:
			void CreateModel();
			void PrepareSimulation();
			LinkUP CreateLinkHierarchy( OpenSim::Body& osBody );

			double integration_accuracy;
			double max_step_size;
			String model_file;

			std::unique_ptr< OpenSim::Model > m_pOsimModel;
			std::unique_ptr< OpenSim::Manager > m_pOsimManager;
			SimTK::State* m_pTkState; // non-owning state reference
			std::unique_ptr< SimTK::Integrator > m_pTkIntegrator;

			class ControllerDispatcher;
			ControllerDispatcher* m_pControllerDispatcher; // owned by m_osModel
		};
	}
}
