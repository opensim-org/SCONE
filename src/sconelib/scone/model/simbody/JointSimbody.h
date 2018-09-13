#pragma once

#include "sim_simbody.h"
#include "scone/model/Joint.h"

namespace OpenSim
{
	class Joint;
}

namespace scone
{
	class SCONE_API JointSimbody : public Joint
	{
	public:
		JointSimbody( Body& body, Joint* parent, class ModelSimbody& model, OpenSim::Joint& osJoint );
		virtual ~JointSimbody();

		virtual const String& GetName() const;

		virtual Vec3 GetPos() const override;
		virtual size_t GetDofCount() const override;
		virtual Real GetDofValue( size_t index = 0 ) const override;
		virtual const String& GetDofName( size_t index = 0 ) const override;

		class ModelSimbody& m_Model;
		OpenSim::Joint& m_osJoint;

		virtual Vec3 GetReactionForce() const override;
	};
}
