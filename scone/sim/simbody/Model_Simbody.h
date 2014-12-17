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
			Model_Simbody();
			virtual ~Model_Simbody();

			bool Load( const String& filename );

			virtual Vec3 GetComPos() override;
			virtual Vec3 GetComVel() override;
			virtual Real GetMass() override;

			/// Get the OpenSim model attached to this model
			OpenSim::Model& GetOpenSimModel() { return *m_osModel; }

		private:
			std::unique_ptr< OpenSim::Model > m_osModel;

			class ControllerDispatcher;
			std::unique_ptr< ControllerDispatcher > m_pControllerDispatcher;
		};
	}
}
