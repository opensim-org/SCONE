#pragma once

#include "sim_simbody.h"
#include "../Model.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"
#include <memory>

namespace OpenSim
{
	class Model;
}

namespace scone
{
	namespace sim
	{
		class SCONE_SIM_SIMBODY_API Model_Simbody : public Model
		{
		public:
			Model_Simbody( class Simulation_Simbody* simulation );
			virtual ~Model_Simbody();

			bool Load( const String& filename );

			virtual Vec3 GetComPos() override;
			virtual Vec3 GetComVel() override;

			virtual Real GetMass() override;

			virtual size_t GetBodyCount() override;
			virtual Body_Simbody& GetBody( size_t idx ) override;

			virtual size_t GetMuscleCount() override;
			virtual Muscle_Simbody& GetMuscle( size_t idx ) override;

			virtual void AddController( ControllerSP controller ) override;

		private:
			class Simulation_Simbody* m_pSimulation;
			std::unique_ptr< OpenSim::Model > m_pModel;

			class ControllerAdapter;
			std::vector< std::unique_ptr< ControllerAdapter > > m_Controllers;
		};
	}
}
