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

			virtual void AdvanceSimulationTo( double time ) override;

			/// Get the OpenSim model attached to this model
			OpenSim::Model& GetOsModel() { return *m_osModel; }
			SimTK::State& GetTkState() { return *m_tkState; }

			virtual void ProcessProperties( const PropNode& props ) override;

		private:
			LinkUP CreateLinkHierarchy( OpenSim::Body& osBody );

			double integration_accuracy;

			std::unique_ptr< OpenSim::Model > m_osModel;
			SimTK::State* m_tkState; // non-owning state reference
			std::unique_ptr< SimTK::Integrator > m_tkIntegrator;

			class ControllerDispatcher;
			std::unique_ptr< ControllerDispatcher > m_pControllerDispatcher;
		};
	}
}
