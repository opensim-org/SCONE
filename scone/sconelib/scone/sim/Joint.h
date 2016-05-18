#pragma once

#include "sim.h"
#include "scone/core/HasName.h"
#include "scone/core/math.h"

namespace scone
{
	namespace sim
	{
		class SCONE_API Joint : public HasName
		{
		public:
			Joint( Body& body, Joint* parent = nullptr );
			virtual ~Joint();

			virtual Vec3 GetPos() const = 0;
			virtual size_t GetDofCount() const = 0;
			virtual Real GetDof( size_t index = 0 ) const = 0;
			virtual const String& GetDofName( size_t index = 0 ) const = 0;
			virtual Vec3 GetReactionForce() const = 0;
			virtual Real GetLoad() const;

			bool HasDof( const String& dof_name ) const;
			const Joint* GetParent() const { return m_pParent; }
			const Body& GetBody() const { return m_Body; }


		protected:
			Body& m_Body;
			Joint* m_pParent;
		};
	}
}
