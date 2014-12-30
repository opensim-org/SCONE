#pragma once

#include "sim_simbody.h"
#include "../Model.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include <memory>
#include "../../core/Factory.h"

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

		class SCONE_SIM_SIMBODY_API Model_Simbody : public Model, public Factoryable< Model, Model_Simbody >
		{
		public:
			Model_Simbody( const String& filename = "" );
			virtual ~Model_Simbody();

			virtual Vec3 GetComPos() override;
			virtual Vec3 GetComVel() override;
			virtual Real GetMass() override;
			virtual Vec3 GetGravity() override;
			virtual bool HasGroundContact() override;

			virtual void AdvanceSimulationTo( double time ) override;
			virtual void WriteStateHistory( const String& file ) override;

			/// Get the OpenSim model attached to this model
			OpenSim::Model& GetOsModel() { return *m_osModel; }
			SimTK::State& GetTkState() { return *m_tkState; }
			void SetTkState( SimTK::State& s ) { m_tkState = &s; }

			virtual void ProcessProperties( const PropNode& props ) override;
			virtual void ProcessParameters( opt::ParamSet& par ) override;
			virtual void Reset() override;

		private:
			void CreateOsModel( const String& file );
			LinkUP CreateLinkHierarchy( OpenSim::Body& osBody );

			double integration_accuracy;
			String model_file;

			std::unique_ptr< OpenSim::Model > m_osModel;
			std::unique_ptr< OpenSim::Manager > m_osManager;
			SimTK::State* m_tkState; // non-owning state reference
			std::unique_ptr< SimTK::Integrator > m_tkIntegrator;

			class ControllerDispatcher;
			ControllerDispatcher* m_pControllerDispatcher; // owned by m_osModel

			class TerminationEventHandler;
			TerminationEventHandler *m_pTerminationEventHandler;
		};
	}
}
