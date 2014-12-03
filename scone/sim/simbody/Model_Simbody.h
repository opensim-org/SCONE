#pragma once

#include "simbody.h"
#include "../Model.h"
#include "Body_Simbody.h"
#include "Muscle_Simbody.h"

namespace scone
{
	namespace sim
	{
		class SIM_SIMBODY_API Model_Simbody : public Model
		{
		public:
			Model_Simbody( class Simulation_Simbody& world );
			virtual ~Model_Simbody();

			bool Load( const String& filename );

			virtual Vec3 GetComPos() override;
			virtual Vec3 GetComVel() override;

			virtual Real GetMass() override;

			virtual size_t GetBodyCount() override;
			virtual Body_Simbody& GetBody( size_t idx ) override;

			virtual size_t GetMuscleCount() override;
			virtual Muscle_Simbody& GetMuscle( size_t idx ) override;

		private:
			class Simulation_Simbody& m_World;
		};
	}
}
