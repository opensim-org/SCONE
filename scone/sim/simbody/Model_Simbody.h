#pragma once

#include "simbody.h"
#include "../Model.h"
#include "Link_Simbody.h"
#include "Muscle_Simbody.h"

namespace scone
{
	namespace sim
	{
		class SIM_SIMBODY_API Model_Simbody : public Model
		{
		public:
			Model_Simbody( class World_Simbody& world );
			virtual ~Model_Simbody();

			bool Load( const String& filename );

			virtual Vec3 GetComPos() override;
			virtual Vec3 GetComVel() override;

			virtual Real GetMass() override;

			virtual size_t GetLinkCount() override;
			virtual Link_Simbody& GetLink( size_t idx ) override;

			virtual size_t GetMuscleCount() override;
			virtual Muscle_Simbody& GetMuscle( size_t idx ) override;

		private:
			class World_Simbody& m_World;
		};
	}
}
